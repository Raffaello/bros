#include <arch/x86/descriptors.h>
#include <bios/vga.h>
#include <arch/x86/defs/interrupts.h>

/*----------------------*
 * GDT defined in GDT.S *
 *----------------------*/
extern void GDT_init();

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

#define GDT_KERNEL_CODE_SEL 0x8            // extracted from GDT.S

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
    IDT_set_IDT_handler(&idtd[numInt], IDT_GATE_INT32, IDT_DPL_RING0, GDT_KERNEL_CODE_SEL, idt_func);
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
            GDT_KERNEL_CODE_SEL,
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
