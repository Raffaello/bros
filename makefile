

# TODO to pass these values to ASM need to convert them to .S files and compiling them wih GCC
#      at that point these can be defined as "defines" from CLI
#      otherwise use NASM/FASM
BIOS_BOOT_SEG=0x7C00
BOOT_REL_SEG=0x600
BOOT2_REL_SEG=${BIOS_BOOT_SEG}
KERNEL_SEG=0x1000
# KERNEL_FILENAME="BROSKRNL.SYS"
# BOOT_RESERVED_SECTORS=3
# FLOPPY_SIZE=1440
# FLOPPY_IMAGE_NAME="br-dos.img"

CC=gcc
SRC_DIR=src/kernel
BUILD_DIR=build
# kernel.c must be the 1st in the list
SRC  = ${SRC_DIR}/kernel.c $(wildcard ${SRC_DIR}/lib/*.c ${SRC_DIR}/cpu/*.c ${SRC_DIR}/bios/*.c wildcard ${SRC_DIR}/drivers/*.c)
OBJS = $(SRC:${SRC_DIR}/%.c=${BUILD_DIR}/%.o)

SRC_S  = $(wildcard ${SRC_DIR}/lib/*.S)
OBJS_S = $(SRC_S:${SRC_DIR}/%.S=${BUILD_DIR}/%.oS)

INCLUDE_DIR=${SRC_DIR}

#CFLAGS+=-Wall -Werror #-Wmissing-prototypes
CFLAGS+=-masm=intel
# CFLAGS+=-o2
CFLAGS+=-std=c17
CFLAGS+=-m32 -ffreestanding -nostartfiles -nostdlib -I ${INCLUDE_DIR}
LFLAGS+=-m elf_i386 # change when starting the kernel in long mode

.PHONY: kernel $(OBJS)

t:
	echo ${SRC}
	echo ${OBJS}
	echo ${SRC_S}
	echo ${OBJS_S}

all: floppy boot2 image kernel

floppy:
	@mkdir -p build
	as -k -o build/boot.o src/bootloader/floppy.asm -I src/bootloader
	ld -o build/boot.out build/boot.o -Ttext ${BOOT_REL_SEG} #-Ttext 0x7c00
	objcopy -O binary -j .text build/boot.out bin/boot.bin

boot2:
	@mkdir -p build
	as -k -o build/boot2.o src/bootloader/boot2.asm -I src/bootloader
	ld -o build/boot2.out build/boot2.o -Ttext ${BOOT2_REL_SEG} #-Ttext 0x600
	objcopy -O binary -j .text build/boot2.out bin/boot2.bin

.SECONDEXPANSION:
$(OBJS): $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.c,$$@)
	@mkdir -p ${@D}
	${CC} $(CFLAGS) -c $^ -o $@

$(OBJS_S): $$(patsubst $(BUILD_DIR)/%.oS,$(SRC_DIR)/%.S,$$@)
	@mkdir -p ${@D}
	${CC} $(CFLAGS) -c $^ -o $@


# bios/vga.c:
# 	${CC} $(CFLAGS) -c  ${SRC_DIR}/$@ -o ${BUILD_DIR}/bios/vga.o

# Monolithic for now
kernel: $(OBJS) ${OBJS_S}
	# gcc -o2 -ffreestanding -nostartfiles -nostdlib -c kernel/kernel.c -o build/kernel.o
	# gcc -m32 -g -ffreestanding -nostartfiles -nostdlib -c src/kernel/kernel.c -o build/kernel.o
	# ${CC} $(CFLAGS) -c ${SRC} -o build/$(@F).o
	
	# ld -m elf_i386 -o build/kernel.out build/kernel.o #-Ttext ${KERNEL_SEG}
	ld $(LFLAGS) -o ${BUILD_DIR}/kernel.out ${OBJS} ${OBJS_S} #-Ttext ${KERNEL_SEG}
	
	objcopy -O binary -j .text build/kernel.out bin/kernel.sys


image: floppy boot2 kernel
	# Using 2 extra Reserved Sectors
	mformat -i br-dos.img -v BROS -B bin/boot.bin -R 3 -f1440 -C
	dd if=bin/boot2.bin of=br-dos.img conv=notrunc seek=1
	mcopy -i br-dos.img bin/kernel.sys ::/BROSKRNL.SYS
	mattrib -i br-dos.img +r +h +s -a /BROSKRNL.SYS
	mdir -i br-dos.img -a


clean:
	rm bin/* -fv
	rm build -fr	v
	rm br-dos.img -fv
