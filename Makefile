# *** Some "defines" *** #

export CC=gcc
export AS=as
export LD=ld

# override with `make BUILD=release`
# default to debug build
export BUILD_TYPE := debug

# using make's computed variables to select object and bin folders
# depending on the build type
BUILD_DIR.debug=build/debug
BIN_DIR.debug=bin/debug
# release
BUILD_DIR.release=build/release
BIN_DIR.release=bin/release
# releae with debug info
BUILD_DIR.reldbg=build/reldbg
BIN_DIR.reldbg=bin/reldbg

export BUILD_DIR=$(BUILD_DIR.$(BUILD_TYPE))
export BIN_DIR=$(BIN_DIR.$(BUILD_TYPE))

export KERNEL_SEG=0x1000
export KERNEL_FILENAME="BROSKRNL.SYS"
export FLOPPY_IMAGE_NAME="br-dos.img"

BIOS_BL_DIR=BIOS_bootloader
KERNEL_DIR=kernel

.PHONY: kernel

all: image

t:
	echo $(BUILD_TYPE)
	echo $(BUILD_DIR)
	echo $(BIN_DIR)

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
