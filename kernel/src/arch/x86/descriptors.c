#include <arch/x86/descriptors.h>
#include <bios/vga.h>
#include <arch/x86/defs/interrupts.h>

/*----------------------*
 * GDT defined in GDT.S *
 *----------------------*/
#define MAX_GDT_DESCRIPTORS 5
                                                                 
// #define GDT_LIMIT(x)            ((uint64_t) x & 0xFFFF) | ((uint64_t)(x & 0xF0000) << (48 - 16))
// #define GDT_BASE(x)             ((uint64_t) (x & 0xFFFFFF) << 16) | ((uint64_t)(x & 0xFF000000) << (16 + 16))
// #define GDT_ATTRIB_READABLE     ((uint64_t) 1 << 41)
// #define GDT_ATTRIB_CONFORMING   ((uint64_t) 1 << 42)
// #define GDT_ATTRIB_CODE         ((uint64_t) 3 << 43)
// #define GDT_ATTRIB_DPL(x)       ((uint64_t) (x & 3) << 45)
// #define GDT_ATTRIB_PRESENT      ((uint64_t) 1 << 47)
// #define GDT_ATTRIB_AVAIL        ((uint64_t) 1 << 52)
// #define GDT_DEFAULT32           ((uint64_t) 1 << 54)
// #define GDT_GRANULARITY         ((uint64_t) 1 << 55)


const static GDT_descriptor_t gdtd[MAX_GDT_DESCRIPTORS] = {
    0x0000000000000000, // null
    0x00CF9A000000FFFF, // Kernel Code
    0x00CF92000000FFFF, // Kernel Data
    0x00CFFA000000FFFF, // User Code
    0x00CFF2000000FFFF  // User Data
};

#define GDT_KERNEL_CODE_SEL ((uint32_t)&gdtd[1] - (uint32_t)&gdtd[0])
#define GDT_KERNEL_DATA_SEL ((uint32_t)&gdtd[2] - (uint32_t)&gdtd[0])


const static struct DT_register_t gdtr = {
    .size   = sizeof(GDT_descriptor_t) * MAX_GDT_DESCRIPTORS - 1,
    .offset = (uint32_t)&gdtd[0]
};

static void GDT_load(const DT_register_t* dtr)
{
    __asm__ volatile ("lgdt %0":: "m"(*dtr));
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

    GDT_load(&gdtr);
    IDT_init();
}
