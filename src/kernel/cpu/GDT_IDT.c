#include <cpu/GDT_IDT.h>
#include <bios/vga.h>

#define GDT_MAX_DESCRIPTORS 3

#define GDT_ACCESS_ACCESSED     0x01
#define GDT_ACCESS_RW           0x02 // Read Write
#define GDT_ACCESS_DC           0x04 // Direction/Conforming
#define GDT_ACCESS_E            0x08 // Executable
#define GDT_ACCESS_S            0x10 // Descriptor Type
#define GDT_ACCESS_DPL          0x60 // Descriptor Privilege Level
#define GDT_ACCESS_P            0x80 // Present

// #define GDT_FLAGS_R             0x1  // Reserved
#define GDT_FLAGS_L             0x2 // Long Mode
#define GDT_FLAGS_DB            0x4 // Size
#define GDT_FLAGS_G             0x8 // Granularity

// NOTE: I had issue reloading a GDT from C, so i have done it trhough ASM
//       Not sure why the gdtd[] isn't initialized, but just zeros
//       forced to call some code to set up and i don't want it.
//       there were some other errors with memory address etc.
//       ASM quite straightforward how to do it.
// CONS: I don't have the CODE_SEG from ASM accessible here.
//       it might force to do IDT into ASM as well.
//       prefer ASM anyway..
extern void GDT_load_asm();


// extern void GDT_reload_segment(uint16_t codeSeg, uint16_t dataSeg); // defined in cpu/GDT_reload_segment.S

// /*__attribute__((aligned(16)))*/ static GDT_descriptor_t gdtd[GDT_MAX_DESCRIPTORS] = {
//     {
//         .limit          = 0,
//         .base_lo        = 0,
//         .base_mi        = 0,
//         .access         = 0,
//         .limit_flags    = 0,
//         .base_hi        = 0
//     },
//     {
//         .limit          = 0xFFFF,
//         .base_lo        = 0,
//         .base_mi        = 0,
//         .access         = GDT_ACCESS_RW | GDT_ACCESS_E | GDT_ACCESS_S | GDT_ACCESS_P,
//         .limit_flags    = ((GDT_FLAGS_DB | GDT_FLAGS_G) << 4) | 0xF,
//         .base_hi        = 0
//     },
//     {
//         .limit          = 0xFFFF,
//         .base_lo        = 0,
//         .base_mi        = 0,
//         .access         = GDT_ACCESS_RW | GDT_ACCESS_E | GDT_ACCESS_S | GDT_ACCESS_P,
//         .limit_flags    = ((GDT_FLAGS_DB | GDT_FLAGS_G) << 4) | 0xF,
//         .base_hi        = 0
//     }
// };

// static DT_register_t gdtr;

// void GDT_load(const DT_register_t* dtr)
// {
//     __asm__("cli");
//     __asm__ volatile("lgdt %0" : : "m"(*dtr));
//     // __asm__("sti");
// }

// static void GDT_set_descriptor(GDT_descriptor_t* gdtd, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
// {
//     gdtd->base_lo = base & 0xFFFF;
//     gdtd->base_mi = (base >> 16) & 0xFF;
//     gdtd->base_hi = (base >> 24) & 0xFF;

//     gdtd->limit         = limit & 0xFFFF;

//     gdtd->limit_flags   = 0;
//     gdtd->limit_flags   = (limit >> 16) & 0x0F;
//     gdtd->limit_flags  |= (flags << 4); 

//     gdtd->access = access;
// }

// initialize gdt
void GDT_init()
{
    // static uint64_t gdtd[3] = {0, 0x00CF9A000000FFFF, 0x00CF9A000000FFFF};
    // static DT_register_t gdtr;

    // GDT_CODE_SEG = GDT_CODE - GDT
    // GDT_DATA_SEG = GDT_DATA - GDT
    // const uint16_t codeSeg = (uint16_t)((uint32_t) &gdtd[1]) - ((uint32_t) &gdtd[0]);
    // const uint16_t dataSeg = (uint16_t)((uint32_t) &gdtd[2]) - ((uint32_t) &gdtd[0]);

    // set up gdtr
    // gdtr.size = (sizeof(GDT_descriptor_t) * GDT_MAX_DESCRIPTORS) - 1; // 8 * 3 - 1 = 23
    // gdtr.offset = ((uint32_t)(&gdtd[0]));
    //null descriptor
    // GDT_set_descriptor(&gdtd[0], 0, 0, 0, 0);
    //default code descriptor
    // GDT_set_descriptor(
    //     &gdtd[1],
    //     0,
    //     0xFFFFF,
    //     GDT_ACCESS_RW | GDT_ACCESS_E | GDT_ACCESS_S | GDT_ACCESS_P,
    //     GDT_FLAGS_DB | GDT_FLAGS_G
    // );
    //default data descriptor
    // GDT_set_descriptor(
    //     &gdtd[2],
    //     0,
    //     0xFFFFF,
    //    GDT_ACCESS_RW | GDT_ACCESS_E | GDT_ACCESS_S | GDT_ACCESS_P,
    //    GDT_FLAGS_DB | GDT_FLAGS_G
    // );

    // GDT_load(&gdtr);
    
    // GDT_reload_segment(codeSeg, dataSeg);

    GDT_load_asm();
}


// ----------------------------------------------------------
// ***                  IDT section                       ***
// ----------------------------------------------------------

#define X86_MAX_INTERRUPTS  256

#define IDT_GATE_TASK       0x5 // 
#define IDT_GATE_INT16      0x6 // 16-bit
#define IDT_GATE_TRAP16     0x7 // 16-bit
#define IDT_GATE_INT32      0xE // 32-bit
#define IDT_GATE_TRAP32     0xF // 32-bit


//interrupt descriptor table
static struct IDT_descriptor_t  idtd[X86_MAX_INTERRUPTS];

//! idtr structure used to help define the cpu's idtr register
static struct DT_register_t     idtr;

//! interrupt handler function type definition
typedef void ((*irq_handler)(void));

void IDT_load(const DT_register_t* dtr)
{
    __asm__("cli");
    __asm__ volatile("lidt %0" : : "m"(idtr));
}

// install a new interrupt handler
static void IDT_install_irq_handler(IDT_descriptor_t* idtd, uint8_t gate_type, uint8_t dpl, uint16_t sel, irq_handler irq)
{
    register uint32_t base = ((uint32_t)&(*irq));

    idtd->base_lo   = base & 0xFFFF;
    idtd->base_hi   = (base >> 16) & 0xFFFF;
    idtd->reserved  = 0;
    idtd->flags     = gate_type | (dpl << 5) | (1 << 7);
    idtd->selector  = sel;
}

void IDT_default_handler()
{
    clearVGA();
    writeVGAChar(0, 0, 'X', 15);
    while(1);
}


void IDT_init(/*uint16_t codeSel*/)
{
    // TODO: not sure now how to segment memory and install interrupt handler..
    //       so for now just basic settings for the function to almost work.

    int code_sel = 0x8;
    // writeVGAChar(20,0,)
    // set up idtr for processor
    idtr.size = sizeof(IDT_descriptor_t) * X86_MAX_INTERRUPTS - 1;
    idtr.offset	= (uint32_t)&idtd[0];

    //register default handlers
    for (int i=0; i<X86_MAX_INTERRUPTS; i++)
    {
        IDT_install_irq_handler(
            &idtd[i],
            IDT_GATE_INT32,
            0,
            code_sel,
            IDT_default_handler
        );
    }

    IDT_load(&idtr);
}
