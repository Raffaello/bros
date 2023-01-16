.PHONY: all kernel

all: floppy boot2 image kernel

floppy:
	as -o build/boot.o bootloader/floppy.asm -I bootloader/
	ld -o build/boot.out build/boot.o -Ttext 0x7c00
	objcopy -O binary -j .text build/boot.out bin/boot.bin

boot2:
	as -o build/boot2.o bootloader/boot2.asm -I bootloader/
	ld -o build/boot2.out build/boot2.o -Ttext 0x600
	objcopy -O binary -j .text build/boot2.out bin/boot2.bin

kernel:
	gcc -o2 -ffreestanding -nostartfiles -nostdlib -c kernel/kernel.c -o build/kernel.o
	ld -o build/kernel.out build/kernel.o -Ttext 0x1000
	objcopy -O binary -j .text build/kernel.out bin/kernel.sys
	


image: floppy boot2 kernel
	# Using 2 Reserved Sectors
	mformat -i br-dos.img -B bin/boot.bin -R 3 -f1440 -C
	dd if=bin/boot2.bin of=br-dos.img conv=notrunc seek=1
	mcopy -i br-dos.img bin/kernel.sys ::/BROSKRNL.SYS
	mattrib -i br-dos.img +r +h +s -a /BROSKRNL.SYS
	mdir -i br-dos.img -a


clean:
	rm bin/* -fv
	rm build/* -fv
	rm br-dos.img -fv
