# *** Some "defines" *** #

export CC=gcc
export AS=as
export LD=ld

export BUILD_DIR=build
export BIN_DIR=bin

export KERNEL_SEG=0x1000
# KERNEL_FILENAME="BROSKRNL.SYS"
export FLOPPY_IMAGE_NAME="br-dos.img"

BIOS_BOOTLOADER=BIOS_bootloader
SRC_DIR=kernel/src

SRC  = $(wildcard \
	${SRC_DIR}/*.c \
	${SRC_DIR}/lib/*.c \
	${SRC_DIR}/cpu/*.c \
	${SRC_DIR}/cpu/mmu//*.c \
	${SRC_DIR}/bios/*.c \
	${SRC_DIR}/drivers/*.c)
OBJS = $(SRC:${SRC_DIR}/%.c=${BUILD_DIR}/%.o)

SRC_S  = $(wildcard \
	${SRC_DIR}/lib/*.S \
	${SRC_DIR}/cpu/*.S \
	${SRC_DIR}/cpu/mmu/*.S \
	${SRC_DIR}/drivers/*.S)
OBJS_S = $(SRC_S:${SRC_DIR}/%.S=${BUILD_DIR}/%.oS)

INCLUDE_DIR=${SRC_DIR}

CFLAGS+=-Wall -Werror #-Wmissing-prototypes
CFLAGS+=-masm=intel
# CFLAGS+=-O2	# optimization are creating issues on the code
CFLAGS+=-g
CFLAGS+=-std=c17
CFLAGS+=-m32 -c -ffreestanding -I ${INCLUDE_DIR} -MMD
CFLAGS+=-nostartfiles -nostdlib
CFLAGS+=-lgcc
CFLAGS+=-DKERNEL_SEG=${KERNEL_SEG} -DSYS_INFO_SEG=${SYS_INFO_SEG}

LFLAGS+=-m elf_i386 # change when starting the kernel in long mode
LFLAGS+=-nostdlib --nmagic
LFLAGS+=-Tkernel.ld # linker option definition file
# LFLAGS+=-Ttext ${KERNEL_SEG}
LFLAGS+=--defsym=KERNEL_SEG=$(KERNEL_SEG)


.PHONY: kernel $(OBJS)

t:
	echo ${SRC}
	echo ${OBJS}
	echo ${SRC_S}
	echo ${OBJS_S}

all: image

bios_bootloader:
	+$(MAKE) -C ${BIOS_BOOTLOADER} all

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

image: bios_bootloader kernel
	mcopy -i ${FLOPPY_IMAGE_NAME} ${BIN_DIR}/kernel.sys ::/BROSKRNL.SYS
	mattrib -i ${FLOPPY_IMAGE_NAME} +r +h +s -a /BROSKRNL.SYS
	mdir -i ${FLOPPY_IMAGE_NAME} -a

gdb-kernel-debug:
	qemu-system-i386 -fda ${FLOPPY_IMAGE_NAME} -S -s &
	gdb ${BUILD_DIR}/kernel.out \
		-ex 'target remote localhost:1234' \
		-ex 'layout src' \
		-ex 'layout reg' \
		-ex 'break _start' \
		-ex 'break *0x7c00' \
		-ex 'b src/kernel/cpu/mmu/VMM.c:74' \
		-ex 'set disassembly-flavor intel' \
		-ex 'continue'

bochs-kernel-debug:
	bochs-debugger -q -f bochs-debugger.rs

clean:
	+$(MAKE) -C BIOS_bootloader clean
	rm ${BIN_DIR}/* -fv
	rm ${BUILD_DIR}/* -frv
	rm ${FLOPPY_IMAGE_NAME} -fv
