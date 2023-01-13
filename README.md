# BROS

BR Operating System.

Educational project.

## Usage

This won't work with the actual RAW 2nd stage bootloader.

- to compile: `make all`
- to run using qemu: `qemu-system-i386 -drive format=raw,file=bin/boot.bin,index=0,if=floppy`

### Create the floppy image

1. create the floppy image: `dd if=/dev/zero of=br-dos.img count=2880 bs=512`
3. create FAT: `mkfs.msdos br-dos.img`
3. copy the boot: `dd if=bin/boot.bin of=br-dos.img conv=notrunc`
4. copy the 2nd stage boot: `dd if=bin/boot2.bin of=br-dos.img conv=notrunc seek=1`

### using a Floppy image

1. run `make image`
2. then run qemu to boot from floppy: `qemu-system-i386 -fda br-dos.img`
