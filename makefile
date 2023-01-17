.PHONY: kernel

# TODO to pass these values to ASM need to convert them to .S files and compiling them wih GCC
#      at that point these can be defined as "defines" from CLI
#      otherwise use NASM
BIOS_BOOT_SEG=0x7C00
BOOT_REL_SEG=0x600
BOOT2_REL_SEG=${BIOS_BOOT_SEG}
KERNEL_SEG=0x1000
# KERNEL_FILENAME="BROSKRNL.SYS"
# BOOT_RESERVED_SECTORS=3
# FLOPPY_SIZE=1440
# FLOPPY_IMAGE_NAME="br-dos.img"

CC=gcc
CFLAGS+=-o2 -m32 -ffreestanding -nostartfiles -nostdlib
LFLAGS+=-m elf_i386 # change when starting the kernel in long mode
SRC=$(wildcard src/**/*.c)
# OBJ = ${SRC:%.c=build/${*F}.o}

all: floppy boot2 image kernel

floppy:
	as -k -o build/boot.o src/bootloader/floppy.asm -I src/bootloader
	ld -o build/boot.out build/boot.o -Ttext ${BOOT_REL_SEG} #-Ttext 0x7c00
	objcopy -O binary -j .text build/boot.out bin/boot.bin

boot2:
	as -k -o build/boot2.o src/bootloader/boot2.asm -I src/bootloader
	ld -o build/boot2.out build/boot2.o -Ttext ${BOOT2_REL_SEG} #-Ttext 0x600
	objcopy -O binary -j .text build/boot2.out bin/boot2.bin

kernel:
	# gcc -o2 -ffreestanding -nostartfiles -nostdlib -c kernel/kernel.c -o build/kernel.o
	#gcc -m32 -g -ffreestanding -nostartfiles -nostdlib -c kernel/kernel.c -o build/kernel.o
	${CC} $(CFLAGS) -c ${SRC} -o build/$(@F).o
	
	#ld -m elf_i386 -o build/kernel.out build/kernel.o #-Ttext ${KERNEL_SEG}
	ld $(LFLAGS) -o build/kernel.out build/kernel.o #-Ttext ${KERNEL_SEG}
	
	objcopy -O binary -j .text build/kernel.out bin/kernel.sys


image: floppy boot2 kernel
	# Using 2 extra Reserved Sectors
	mformat -i br-dos.img -B bin/boot.bin -R 3 -f1440 -C
	dd if=bin/boot2.bin of=br-dos.img conv=notrunc seek=1
	mcopy -i br-dos.img bin/kernel.sys ::/BROSKRNL.SYS
	mattrib -i br-dos.img +r +h +s -a /BROSKRNL.SYS
	mdir -i br-dos.img -a


clean:
	rm bin/* -fv
	rm build/* -fv
	rm br-dos.img -fv
