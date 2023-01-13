all: floppy boot2
    # hack to avoid creating the floppy image at the moment, dirty and quicker for test...
	# cat bin/boot.bin bin/boot2.bin > bin/bootloader.bin

floppy:
	as -o bin/boot.o bootloader/floppy.asm
	ld -o bin/boot.out bin/boot.o -Ttext 0x7c00
	objcopy -O binary -j .text bin/boot.out bin/boot.bin

boot2: 
	as -o bin/boot2.o bootloader/boot2.asm
	ld -o bin/boot2.out bin/boot2.o -Ttext 0x0
	objcopy -O binary -j .text bin/boot2.out bin/boot2.bin

clean:
	rm bin/*
