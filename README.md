# BROS

BR Operating System.

## Info / Abstract / Synopsys

The project uses the legacy technology based on BIOS, a roughly between 20-40 years old tech.

In the "next phase", it might be consider to do a UEFI bootloader, but i might be more interested 
dealing with `long mode` (x64) and multi-core CPUs.

---

There are too many aspects to work on in a modern OS, that just requires years to develop and multiple contributions.

--- 

Aware of that,

The main goal of this project and its evolution is just for **learning** purposes.

## The Bootloader

It boots from a FAT12 Boot sector (0).

It is self-relocating to `0x600` memory region and chain loading the FAT extra Reserved Sectors in his previous address at `0x7C00` using BIOS interrupts for screen and disk I/O.

The first section of the boot-loader is passing the boot drive number in AL register.

So when jumping to the 2nd stage, that information is passed on through a CPU Register (AL).

---

The 2nd stage is searching the Kernel file in the FAT12 filesystem and loading it at `0x1000`.

Then It switches the CPU to Protected Mode (32 bits) and enabling A20 Gate (addressing up to 4GB RAM),
 then it executes the kernel.

**NOTE:**

The FAT12 is using few extra reserved sectors, despite the basic bootloader could have been fit in the first sector only,
but because of performing chain-loading, adding extra information and making it verbose, extra sectors have been used.

The FAT allow that, so just took advantages of it.


### Multi-Boot (maybe in the future, for now is a "proprietary format")

The 2nd stage bootloader is not a multi-boot loader, but it uses some of its specification idea to communicate correctly
to the kernel, for e.g., passing the Total amount of RAM installed, some "magic" values to recognized the kernel was loaded from its official bootloader and so on:

```c
EAX = 'BROS' // magic string
EBX = 0x600  // address of a struct for system information (a.k.a multiboot). (mostly will be `0x600`) (not really required).
```

The system info struct is something like (it might evolve as needed, TODO: align in 16/32 bits):
```c
uint32_t        begin_marker;                        // 4 bytes magic marker to indicate start
uint32_t        total_ram;                           // total_mem in KB less 1MB, expect max value (0x3FFC00) (4GB), above this value won't be used.
                                                     // a value of 0 represent an error at BIOS level and should trigger a kernel panic or running is own
                                                     // routines to detect RAM.
uint8_t         boot_device;                         // boot "letter"
uint8_t         num_mem_map_entries                  // BIOS MEM_MAP entries 24 bytes each in size
MEM_MAP_ENTRIES mem_map_entries[num_mem_map_entries] // array of mem map entries
uint32_t        end_marker;                          // 4 bytes magic marker to indicate end
```

The `MEM_MAP_ENTRIES` are in the format of the BIOS INT 15h, AX=E820h.
```c
uint32_t base_addr_lo;
uint32_t base_addr_hi;
uint32_t length_lo;
uint32_t length_hi;
uint32_t type;
uint32_t acpi;
```

where `type` is one of the following:

```c
AVAILABLE      1
RESERVED       2
ACPI_RECLAIM   3
ACPI_NVS       4
```

### Memory Mapping (Bootloader section)

The actual memory mapping is:

1. The Boot sector is loaded by the BIOS at the default location `0x7C00` and self-relocating at `0x600` (like a MBR would do).

2. The 2nd stage Boot is loaded at `0x7C00`, at the original boot memory location.


    This step uses the `0x1000` location for loading the Fat Root Directory, then the FAT and finaly the kernel.

    The stack during the bootloader process is set to start from `0x600-2` = `0x5FE`

    Also a buffer at `0x600` is used to store the values from the FAT linked list to read the file from disk,

    so after have scanned the linked list of the file, the FAT table can be disposed and it is replaced by the kernel itself.


3. Finaly the kernel is loaded at `0x1000`, after that is executed.


## The Kernel (32 bits)

The kernel is a file in the FAT12, so it is a physical file on the floppy image.

The name must be `BROSKRNL.SYS` with Hidden, System, Read-Only attributes.

The kernel is a 32 bit executable and therefore need at least a `80386` CPU running in protected mode.

At its very entry point, it performs some validation checks, it has been loaded correctly from the bootloader and retrieving some information passed by the bootloader at some given memory location.

 It will also perform the re-initialization of protected memory and interrupts as a per Intel specs, also setting up the `PIC` and the `PIT`.

 **NOTE:**

The same thing of "storing the file on the filesystem (FAT12)" could have been done similarly for the bootloader (2nd stage), the boot sector loads the bootloader, but i prefered to exploit the FAT filesystem using few extra reserved sectors, instead to store the extra bootloader code as a file.

 ### Memory Mapping

The kernel runs in protected mode (`PM`) with paging (`PG`) of 4 KB size.

> TODO ...



## Usage

TODO: test it with VirtualBox.

### Requirements

- GCC
- AS (GNU GAS)
- make
- qemu / bochs
- mtools (mformat, mcopy, mdir, mattrib)

### Compiling

- To compile: `make all`
- To just create the Floppy image instead: `make image`

### Running

- to run using `qemu`: `qemu-system-i386 -fda br-dos.img` or similar.
- to run with  `bochs`: `bochs -q -f bochs.rs` or similar.
- to run with `bochs` debbugger: `bochs-debugger -q -f bochs.rs` or similar

To debug can be used `gdb`: `target remote localhost:1234`

## Note

- GCC inline assembly is just silly, must be checked as is generating stupid asm.
- consider switching from GAS to NASM
- eventually consider supporting MSVC too as exercise.
- consider doing C++  along side C kernel
- consider doing Rust along side C kernel as well..
