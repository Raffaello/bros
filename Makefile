

# TODO to pass these values to ASM need to convert them to .S files and compiling them wih GCC
#      at that point these can be defined as "defines" from CLI
#      otherwise use NASM/FASM (suggested NASM)
FAT_RESERVED_SECTORS=4
BIOS_BOOT_SEG=0x7C00
BOOT_REL_SEG=0x600
BOOT2_SEG=${BIOS_BOOT_SEG}
KERNEL_SEG=0x1000
# KERNEL_FILENAME="BROSKRNL.SYS"
# FLOPPY_SIZE=1440
FLOPPY_IMAGE_NAME="br-dos.img"
SYS_INFO_SEG=0x600 # same as BOOT_REL_SEG as it won't be used anymore

CC=gcc
AS=as
LD=ld

SRC_DIR=src/kernel
BUILD_DIR=build
BIN_DIR=bin

SRC  = ${SRC_DIR}/kernel.c $(wildcard ${SRC_DIR}/lib/*.c ${SRC_DIR}/cpu/*.c ${SRC_DIR}/bios/*.c wildcard ${SRC_DIR}/drivers/*.c)
OBJS = $(SRC:${SRC_DIR}/%.c=${BUILD_DIR}/%.o)

SRC_S  = $(wildcard ${SRC_DIR}/lib/*.S ${SRC_DIR}/cpu/*.S ${SRC_DIR}/drivers/*.S)
OBJS_S = $(SRC_S:${SRC_DIR}/%.S=${BUILD_DIR}/%.oS)

INCLUDE_DIR=${SRC_DIR}

ASFLAGS+=--fatal-warnings -n --32 -march=i386 -I src/bootloader
AS_LFLAGS+=-m elf_i386

CFLAGS+=-Wall -Werror #-Wmissing-prototypes
CFLAGS+=-masm=intel
# CFLAGS+=-o2
CFLAGS+=-g
CFLAGS+=-std=c17
CFLAGS+=-m32 -c -ffreestanding -I ${INCLUDE_DIR}
CFLAGS+=-nostartfiles -nostdlib
CFLAGS+=-lgcc
CFLAGS+=-DKERNEL_SEG=${KERNEL_SEG} -DSYS_INFO_SEG=${SYS_INFO_SEG}

LFLAGS+=-m elf_i386 # change when starting the kernel in long mode
LFLAGS+=-nostdlib --nmagic
LFLAGS+=-Tlinker.ld # linker option definition file
# LFLAGS+=-Ttext ${KERNEL_SEG}
LFLAGS+=--defsym=KERNEL_SEG=$(KERNEL_SEG)

.PHONY: kernel $(OBJS)

t:
	echo ${SRC}
	echo ${OBJS}
	echo ${SRC_S}
	echo ${OBJS_S}

all: floppy boot2 image kernel

floppy:
	@mkdir -p build
	${AS} ${ASFLAGS} -o build/boot.o src/bootloader/floppy.asm
	${LD} ${AS_LFLAGS} -o build/boot.out build/boot.o -Ttext ${BOOT_REL_SEG}
	objcopy -O binary -j .text build/boot.out ${BIN_DIR}/boot.bin

boot2:
	@mkdir -p build
	${AS} ${ASFLAGS} -o build/boot2.o src/bootloader/boot2.asm
	${LD} ${AS_LFLAGS} -o build/boot2.out build/boot2.o -Ttext ${BOOT2_SEG}
	objcopy -O binary -j .text build/boot2.out ${BIN_DIR}/boot2.bin

.SECONDEXPANSION:
$(OBJS): $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.c,$$@)
	@mkdir -p ${@D}
	${CC} $(CFLAGS) -c $^ -o $@

$(OBJS_S): $$(patsubst $(BUILD_DIR)/%.oS,$(SRC_DIR)/%.S,$$@)
	@mkdir -p ${@D}
	${CC} $(CFLAGS) $^ -o $@

# Monolithic for now
kernel: $(OBJS) ${OBJS_S}
	# ${LD} $(LFLAGS) -o ${BUILD_DIR}/kernel.out ${OBJS} ${OBJS_S}
	# objcopy -O binary  -j .text ${BUILD_DIR}/kernel.out ${BIN_DIR}/kernel.sys
	# ${LD} $(LFLAGS) -o ${BIN_DIR}/kernel.sys ${OBJS} ${OBJS_S}
	${LD} $(LFLAGS) -o ${BUILD_DIR}/kernel.out ${OBJS} ${OBJS_S}
	objcopy -O binary  -j .text ${BUILD_DIR}/kernel.out ${BIN_DIR}/kernel.sys

image: floppy boot2 kernel
	# Using 2 extra Reserved Sectors
	mformat -i ${FLOPPY_IMAGE_NAME} -v BROS -B ${BIN_DIR}/boot.bin -R ${FAT_RESERVED_SECTORS} -f1440 -C
	dd if=${BIN_DIR}/boot2.bin of=${FLOPPY_IMAGE_NAME} conv=notrunc seek=1
	mcopy -i ${FLOPPY_IMAGE_NAME} ${BIN_DIR}/kernel.sys ::/BROSKRNL.SYS
	mattrib -i ${FLOPPY_IMAGE_NAME} +r +h +s -a /BROSKRNL.SYS
	mdir -i ${FLOPPY_IMAGE_NAME} -a

gdb-kernel-debug: image
	qemu-system-i386 -fda ${FLOPPY_IMAGE_NAME} -S -s &
	gdb ${BUILD_DIR}/kernel.out \
		-ex 'target remote localhost:1234' \
		-ex 'layout src' \
		-ex 'layout reg' \
		-ex 'break _start' \
		-ex 'break *0x7c00' \
		-ex 'set disassembly-flavor intel' \
		-ex 'continue'

clean:
	rm ${BIN_DIR}/* -fv
	rm ${BUILD_DIR}/* -frv
	rm ${FLOPPY_IMAGE_NAME} -fv
