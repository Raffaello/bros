# BROS

BR Operating System.

Educational project.


## Bootsector specification

This is based on `FAT16` filesystem.

|Offset|Size|Section |Contents                        |Typical value|
|------|----|--------|--------------------------------|-------------|
|0000  |3   |Code    |Jump to rest of code            | 
|0003  |8   |BPB     |OEM name                        |
|0011  |2   |        |Bytes per sector                |512|
|0013  |1   |        |Number of sectors per cluster   |1|
|0014  |2   |        |Number of reserved sectors      |1|
|0016  |1   |        |Number of FAT tables            |2|
|0017  |2   |        |Number of root directory entries|224|
|0019  |2   |        |Total number of sectors         |2880|
|0021  |1   |        |Media descriptor                |0xf0|
|0022  |2   |        |Number of sectors per FAT       |9|
|0024  |2   |        |Number of sectors/track         |9|
|0026  |2   |        |Number of heads                 |2|
|0028  |2   |        |Number of hidden sectors        |0|
|0030  |2   |EBPB    |Number of hidden sectors (high word)|0|
|0032  |4   |        |Total number of sectors in filesystem|	 
|0036  |1   |        |Logical drive number            |0|
|0037  |1   |        |Reserved                        |	 
|0038  |1   |        |Extended signature              |0x29|
|0039  |4   |        |Serial number                   |	 
|0043  |11  |        |Volume label                    |MYVOLUME|
|0054  |8   |	     |Filesystem type                 |FAT16|
|0062  |448 |Code    |Boot code                       |	 
|0510  |2   |Required|Boot signature                  |0xaa55|


## Usage

- to compile: `make bootstrap_floppy`
- to run using qemu: `qemu-system-i386 -drive format=raw,file=bin/boot.bin,index=0,if=floppy`

### Create the floppy image

1. create the floppy image: `dd if=/dev/zero of=br-dos.img count=50 bs=1M`
2. create the partion: `fdisk br-dos.img` then `o`, `n`,`p`,`1`,`2048`,`99999`,`t`,`4`,`w`
3. create FAT fs: `mkfs.vfat br-dos.img`

### using a Floppy image

1. copy the compiled `bin/boot.bin` into the floppy image:
```bash
dd if=bin/boot.bin of=br-dos.img bs=512 count=1
```
2. then run qemu to boot from floppy: `qemu-system-i386 -fda br-dos.img`



## Documentation:

- http://independent-software.com/operating-system-development.html
- https://en.wikibooks.org/wiki/QEMU/Images
- https://www.viralpatel.net/taj/tutorial/hello_world_bootloader.php
- https://wiki.osdev.org/Rolling_Your_Own_Bootloader
- https://wiki.osdev.org/Bootloader_FAQ
- http://www.cs.cmu.edu/~410-s07/p4/p4-boot.pdf