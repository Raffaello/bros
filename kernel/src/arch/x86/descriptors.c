#include <arch/x86/descriptors.h>
#include <bios/vga.h>

/*-----*
 * GDT *
 *-----*/

// #define GDT_MAX_DESCRIPTORS 3

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


// TODO: review later...
// extern void GDT_load_asm();
extern void GDT_init();

// initialize gdt
// static void GDT_init()
// {
//     // static uint64_t gdtd[3] = {0, 0x00CF9A000000FFFF, 0x00CF9A000000FFFF};
//     // static DT_register_t gdtr;

//     GDT_load_asm();
// }


// ----------------------------------------------------------
// ***                  IDT section                       ***
// ----------------------------------------------------------
#define MAX_INTERRUPTS  256

#define IDT_GATE_TASK       0x5 // 
#define IDT_GATE_INT16      0x6 // 16-bit
#define IDT_GATE_TRAP16     0x7 // 16-bit
#define IDT_GATE_INT32      0xE // 32-bit
#define IDT_GATE_TRAP32     0xF // 32-bit

#define IDT_DPL_RING0       0   // no bits
#define IDT_DPL_RING1       2   // bit 1  (looks weird ring 1 & 2 have that bits swapped...)
#define IDT_DPL_RING2       1   // bit 0
#define IDT_DPL_RING3       3   // both bits

#define CODE_SEL 0x8

//interrupt descriptor table
static struct /*__attribute__((aligned(16)))*/ IDT_descriptor_t  idtd[MAX_INTERRUPTS];
static struct DT_register_t     idtr;

static void IDT_load(const DT_register_t* dtr)
{
    __asm__("cli");
    __asm__ volatile("lidt %0" : : "m"(idtr));
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
    IDT_set_IDT_handler(&idtd[numInt], IDT_GATE_INT32, IDT_DPL_RING0, CODE_SEL, idt_func);
}

static void IDT_init()
{
    // set up idtr for processor
    idtr.size = sizeof(IDT_descriptor_t) * MAX_INTERRUPTS - 1;
    idtr.offset	= (uint32_t)&idtd[0];

    //register default handlers
    for (int i=0; i < MAX_INTERRUPTS; i++)
    {
        IDT_set_IDT_handler(
            &idtd[i],
            IDT_GATE_INT32,
            IDT_DPL_RING0,
            CODE_SEL,
            IDT_default_handler
        );
    }

    IDT_load(&idtr);
}

void init_descriptor_tables()
{
    GDT_init();
    IDT_init();
}
