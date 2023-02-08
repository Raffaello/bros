#include <arch/x86/descriptors.h>
#include <bios/vga.h>
#include <arch/x86/defs/interrupts.h>

/*----------------------*
 * GDT defined in GDT.S *
 *----------------------*/
#define MAX_GDT_DESCRIPTORS 5
                                                                 
// const static GDT_descriptor_t gdtd[MAX_GDT_DESCRIPTORS] = {
//     0x0000000000000000, // null
//     0x00CF9A000000FFFF, // Kernel Code
//     0x00CF92000000FFFF, // Kernel Data
//     0x00CFFA000000FFFF, // User Code
//     0x00CFF2000000FFFF  // User Data
// };

#define GDT_CODE_ATTR_READABLE      1 << 1
#define GDT_CODE_ATTR_CONFORMING    1 << 2
#define GDT_CODE_ATTR               3 << 3
#define GDT_CODE_ATTR_DPL(x)        (x & 3) << 5
#define GDT_CODE_ATTR_PRESENT       1 << 7
// #define GDT_ATTR_AVAIL       
#define GDT_CODE_DEFAULT32          1 << 6
#define GDT_CODE_GRANULARITY        1 << 7

#define GDT_DATA_ATTR_WRITABLE      1 << 1
#define GDT_DATA_ATTR_EXPDOWN       1 << 2
#define GDT_DATA_ATTR               2 << 3
#define GDT_DATA_ATTR_DPL(x)        GDT_CODE_ATTR_DPL(x)
#define GDT_DATA_ATTR_PRESENT       GDT_CODE_ATTR_PRESENT
#define GDT_DATA_BIG                1 << 6           
#define GDT_DATA_GRANULAIRTY        GDT_CODE_GRANULARITY

#define GDT_DPL_KERNEL              0
#define GDT_DPL_DRIVER1             1
#define GDT_DPL_DRIVER2             2
#define GDT_DPL_USER                3

const static GDT_descriptor_t gdtd[MAX_GDT_DESCRIPTORS] = {
    {0}, // null-descriptor
    {
        .limit = 0xFFFF,
        .base_lo = 0,
        .base_mi = 0,
        .attr = GDT_CODE_ATTR_READABLE | GDT_CODE_ATTR | GDT_CODE_ATTR_DPL(GDT_DPL_KERNEL) | GDT_CODE_ATTR_PRESENT, //0x9A,
        .limit_attr = 0xF | GDT_CODE_DEFAULT32 | GDT_CODE_GRANULARITY
    },
    {
        .limit = 0xFFFF,
        .base_lo = 0,
        .base_mi = 0,
        .attr = GDT_DATA_ATTR_WRITABLE | GDT_DATA_ATTR | GDT_DATA_ATTR_DPL(GDT_DPL_KERNEL) | GDT_DATA_ATTR_PRESENT, //0x92,
        .limit_attr = 0xF | GDT_DATA_BIG | GDT_DATA_GRANULAIRTY //0xCF
    },
    {
        .limit = 0xFFFF,
        .base_lo = 0,
        .base_mi = 0,
        .attr = GDT_CODE_ATTR_READABLE | GDT_CODE_ATTR | GDT_CODE_ATTR_DPL(GDT_DPL_USER) | GDT_CODE_ATTR_PRESENT, //0xFA,
        .limit_attr = 0xF | GDT_CODE_DEFAULT32 | GDT_CODE_GRANULARITY
    },
    {
        .limit = 0xFFFF,
        .base_lo = 0,
        .base_mi = 0,
        .attr = GDT_DATA_ATTR_WRITABLE | GDT_DATA_ATTR | GDT_DATA_ATTR_DPL(GDT_DPL_USER) | GDT_DATA_ATTR_PRESENT, //0xF2,
        .limit_attr =  0xF | GDT_DATA_BIG | GDT_DATA_GRANULAIRTY
    }
};

#define GDT_KERNEL_CODE_SEL ((uint32_t)&gdtd[1] - (uint32_t)&gdtd[0])
#define GDT_KERNEL_DATA_SEL ((uint32_t)&gdtd[2] - (uint32_t)&gdtd[0])

const static struct DT_register_t gdtr = {
    .size   = sizeof(GDT_descriptor_t) * MAX_GDT_DESCRIPTORS - 1,
    .offset = (uint32_t)&gdtd[0]
};

static inline void GDT_load(const DT_register_t* dtr)
{
    __asm__ volatile ("lgdt %0":: "m"(*dtr));
}

__attribute__((naked))
static void GDT_reload_segments()
{
    __asm__ volatile(
        "jmp %0:__GDT_reload_segments\n"
        "__GDT_reload_segments:\n"
        "mov ax, %1\n"
        "mov ds, ax\n"
        "mov es, ax\n"
        "mov fs, ax\n"
        "mov gs, ax\n"
        "mov ss, ax\n"
        "ret"
    ::"i"(GDT_KERNEL_CODE_SEL), "i"(GDT_KERNEL_DATA_SEL));
}

static inline void GDT_init()
{
    GDT_load(&gdtr);
    GDT_reload_segments();
}

// ----------------------------------------------------------
// ***                  IDT section                       ***
// ----------------------------------------------------------

#define IDT_GATE_TASK       0x5 // 
#define IDT_GATE_INT16      0x6 // 16-bit
#define IDT_GATE_TRAP16     0x7 // 16-bit
#define IDT_GATE_INT32      0xE // 32-bit
#define IDT_GATE_TRAP32     0xF // 32-bit

#define IDT_DPL_RING0       0   // no bits
#define IDT_DPL_RING1       1   // bit 1
#define IDT_DPL_RING2       2   // bit 0
#define IDT_DPL_RING3       3   // both bits


//interrupt descriptor table
static struct /*__attribute__((aligned(16)))*/ IDT_descriptor_t  idtd[MAX_INTERRUPTS];

const static struct DT_register_t idtr = {
    .size   = sizeof(IDT_descriptor_t) * MAX_INTERRUPTS - 1,
    .offset = (uint32_t)&idtd[0]
};

static inline void IDT_load(const DT_register_t* dtr)
{
    __asm__ volatile("lidt %0" : : "m"(*dtr));
}

// install a new interrupt handler
static void IDT_set_IDT_handler(IDT_descriptor_t* idtd, uint8_t gate_type, uint8_t dpl, uint16_t sel, IDT_Handler idt)
{
    register uint32_t base = ((uint32_t)&(*idt));

    idtd->base_lo   = base & 0xFFFF;
    idtd->base_hi   = (base >> 16) & 0xFFFF;
    idtd->reserved  = 0;
    idtd->flags     = gate_type | (dpl << 5) | (1 << 7);
    idtd->selector  = sel;
}

void IDT_default_handler()
{
    VGA_clear();
    VGA_WriteChar(0, 0, 'X', 15);
    while(1);
}

void IDT_set_gate(const uint8_t numInt, IDT_Handler idt_func)
{
    IDT_set_IDT_handler(&idtd[numInt], IDT_GATE_INT32, IDT_DPL_RING0, GDT_KERNEL_CODE_SEL, idt_func);
}

static void IDT_init()
{
    //register default handlers
    for (int i=0; i < MAX_INTERRUPTS; i++)
    {
        IDT_set_IDT_handler(
            &idtd[i],
            IDT_GATE_INT32,
            IDT_DPL_RING0,
            GDT_KERNEL_CODE_SEL,
            IDT_default_handler
        );
    }

    IDT_load(&idtr);
}

void init_descriptor_tables()
{
    __asm__("cli");

    GDT_init();
    IDT_init();
}
