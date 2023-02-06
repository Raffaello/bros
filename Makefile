# *** Some "defines" *** #

export CC=gcc
export AS=as
export LD=ld

export BUILD_DIR=build
export BIN_DIR=bin

export KERNEL_SEG=0x1000
export KERNEL_FILENAME="BROSKRNL.SYS"
export FLOPPY_IMAGE_NAME="br-dos.img"

BIOS_BL_DIR=BIOS_bootloader
KERNEL_DIR=kernel

.PHONY: kernel

all: image

bios_bootloader:
	+$(MAKE) -C ${BIOS_BL_DIR} all

kernel: # bios_bootloader
	+$(MAKE) -C ${KERNEL_DIR} all

image: bios_bootloader kernel
	mdir -i ${FLOPPY_IMAGE_NAME} -a

gdb-kernel-debug:
	qemu-system-i386 -fda ${FLOPPY_IMAGE_NAME} -S -s &
	gdb ${KERNEL_DIR}/${BUILD_DIR}/kernel.out \
		-ex 'target remote localhost:1234' \
		-ex 'layout src' \
		-ex 'layout reg' \
		-ex 'break _start' \
		-ex 'break *0x7c00' \
		-ex 'b ${KERNEL_DIR}/src/arch/x86/mmu/VMM.c:74' \
		-ex 'set disassembly-flavor intel' \
		-ex 'continue'

bochs-debug:
	bochs-debugger -q -f bochs-debugger.rs

clean:
	+$(MAKE) -C ${BIOS_BL_DIR} clean
	+$(MAKE) -C ${KERNEL_DIR} clean
	rm ${BIN_DIR}/* -fv
	rm ${FLOPPY_IMAGE_NAME} -fv
