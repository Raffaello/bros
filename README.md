# BROS

BR Operating System.

Educational project.

## Info / Abstract / Synopsys

The project uses the legacy technology based on BIOS, a roughly 30-40 years old tech.

In the "next phase", it might be consider to do a UEFI bootloader, but i might be more interested 
dealing with `long mode` (x64) and multi-core CPUs.

---

There are too many aspects to work on in a modern OS, that just requires years to develop anyway and multiple contributions.

--- 

Aware of that,

The main goal of this project and its evolution is just for **learning** purposes.

## The Bootloader

It boots from a FAT12 Boot sector (0), chain loading the FAT extra Reserved Sector(s), using BIOS interrupts.

Then it will pre-load the kernel and switch the CPU to Protected MODE (32 bits) addressing up to 4GB RAM,
to able to execute the kernel.

The FAT12 used has 3 reserved sector, despite the basic bootloader could have been fit in the first sector only,
Due to perform a chain-loading, add extra information and make it more verbose, an extra 1024KB, 2 sectors,
have been used.
The FAT allow that, so just took advantages of it.

## The Kernel

The kernel is a file in the FAT12, so it is a phisical file on the floppy image.

The name must be `BROSKRNL.SYS` with Hidden, System, Read-Only attributes.

The kernel is a 32 bit executable and therefore need at least a `80386` CPU running in protected mode

**NOTE:**

The same thing could have been done similarly for the bootloader (2nd stage), the boot sector loads the bootloader, but i have prefered to exploit the FAT filesystem using few extra reserved sectors instead to store (hide) the extra bootloader code.

## Memory Mapping

The actual memory mapping (and already on revision) is:

1. The Boot sector is loaded by the BIOS at the default location `0x7C00` (at the moment I am considering a self-relocating boot sector).

2. The 2nd stage Boot is loaded at `0x600`, this should be changed, as it is better going up in memory addressing rather then backward.

3. Finaly the kernel is loaded at `0x1000`, after that the kernel should self relocate to upper memory.

## Usage

### Requirements

- GCC
- make
- qemu
- dd
- mtools (mformat, mcopy, mdir, mattrib)

### Compiling

- to compile: `make all`
- to run using qemu: `qemu-system-i386 -fda br-dos.img` or similar.

To just create the Floppy image instead: `make image`
