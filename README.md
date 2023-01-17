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

It boots from a FAT12 Boot sector (0).

It is self-relocating to `0x600` memory region and chain loading the FAT extra Reserved Sector(s) in his previous address `0x7C00`.

It is using BIOS interrupts for screen and disk I/O.

---

The chained loaded 2nd stage,
it searching the Kernel file in the FAT and loading it.

Then switch the CPU to Protected MODE (32 bits) addressing up to 4GB RAM (A20 Gate),
to be able to execute the kernel.

**NOTE:**

The FAT12 used has 3 reserved sector, despite the basic bootloader could have been fit in the first sector only,
Due to performing a chain-loading, adding extra information and making it verbose, an extra 1024KB (2 sectors)
have been used.
The FAT allow that, so just took advantages of it.

## The Kernel

The kernel is a file in the FAT12, so it is a phisical file on the floppy image.

The name must be `BROSKRNL.SYS` with Hidden, System, Read-Only attributes.

The kernel is a 32 bit executable and therefore need at least a `80386` CPU running in protected mode.

**NOTE:**

The same thing could have been done similarly for the bootloader (2nd stage), the boot sector loads the bootloader, but i have prefered to exploit the FAT filesystem using few extra reserved sectors instead to store the extra bootloader code as a file.

## Memory Mapping

The actual memory mapping (and already on revision) is:

1. The Boot sector is loaded by the BIOS at the default location `0x7C00` and self-relocating at `0x600` (like a MBR would do).

2. The 2nd stage Boot is loaded at `0x7C00`, the original boot memory location.


    This step uses the `0x1000` location for loading the Fat Root Directory, then the FAT and finaly the kernel.

    The stack during the bootloader process is set to start from `0x600-2` = `0x5FE`

    Also a buffer at `0x600` is used to store the values from the FAT linked list to read the file from disk,

    so after have scanned the linked list of the file, the FAT table can be disposed and it is replaced by the kernel itself.


3. Finaly the kernel is loaded at `0x1000`, after that the kernel should self relocate to upper memory.

## Usage

### Requirements

- GCC
- AS (GNU GAS)
- make
- qemu
- dd
- mtools (mformat, mcopy, mdir, mattrib)

### Compiling

- to compile: `make all`
- to run using qemu: `qemu-system-i386 -fda br-dos.img` or similar.

To just create the Floppy image instead: `make image`
