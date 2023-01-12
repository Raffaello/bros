floppy:
	as -o bin/boot.o bootloader/floppy.asm
	ld -o bin/boot.out bin/boot.o -Ttext 0x7c00
	objcopy -O binary -j .text bin/boot.out bin/boot.bin

clean:
	rm bin/*
