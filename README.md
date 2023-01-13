# BROS

BR Operating System.

Educational project.

## Usage

- to compile: `make all`
- to run using qemu: `qemu-system-i386 -drive format=raw,file=bin/boot.bin,index=0,if=floppy`

### Create the floppy image

1. create the floppy image: `dd if=/dev/zero of=br-dos.img count=2880 bs=512`
3. create FAT: `mkfs.msdos br-dos.img`
3. copy the boot: `dd if=bin/boot.bin of=br-dos.img conv=notrunc`
4. copy the 2nd stage boot: `dd if=bin/boot2.bin of=br-dos.img conv=notrunc seek=1`


### using a Floppy image

1. copy the compiled `bin/boot.bin` into the floppy image:
```bash
dd if=bin/boot.bin of=br-dos.img bs=512 count=1
dd if=bin/boot2.bin of=br-dos.img bs=512 count=1 skip=1
```
2. then run qemu to boot from floppy: `qemu-system-i386 -fda br-dos.img`



## Documentation:

- http://independent-software.com/operating-system-development.html
- https://en.wikibooks.org/wiki/QEMU/Images
- https://www.viralpatel.net/taj/tutorial/hello_world_bootloader.php
- https://wiki.osdev.org/Rolling_Your_Own_Bootloader
- https://wiki.osdev.org/Bootloader_FAQ
- http://www.cs.cmu.edu/~410-s07/p4/p4-boot.pdf