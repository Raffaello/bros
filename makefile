all: floppy boot2 image
	cat bin/boot.bin bin/boot2.bin > bin/bootloader.bin

floppy:
	as -o bin/boot.o bootloader/floppy.asm
	ld -o bin/boot.out bin/boot.o -Ttext 0x7c00
	objcopy -O binary -j .text bin/boot.out bin/boot.bin

boot2:
	as -o bin/boot2.o bootloader/boot2.asm
	ld -o bin/boot2.out bin/boot2.o -Ttext 0x600
	objcopy -O binary -j .text bin/boot2.out bin/boot2.bin

image: floppy boot2
	dd if=/dev/zero of=br-dos.img count=2880 bs=512
	mkfs.fat -F 12 br-dos.img
	dd if=bin/boot.bin of=br-dos.img conv=notrunc
	dd if=bin/boot2.bin of=br-dos.img conv=notrunc seek=1

clean:
	rm bin/*
