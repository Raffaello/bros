#include <drivers/keyboard.h>
#include <arch/x86/io.h>
#include <arch/x86/ISR_IRQ.h>
#include <arch/x86/defs/IRQ.h>
#include <lib/conio.h>
// #include <bios/vga.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS    0x64

static void _keyboard_handler(ISR_registers_t r)
{
    const uint8_t c = read_scan_code();
    r.eax           = c;
    CON_putc(c);
    // VGA_WriteChar(0, 24, c, VGA_COLOR_GRAY);
}

void keyboard_init(void)
{
    [[maybe_unused]] const uint8_t c = read_scan_code();
    IRQ_register_interrupt_handler(IRQ_KEYBOARD, _keyboard_handler);
}

uint8_t read_scan_code(void)
{
    return inb(KEYBOARD_DATA_PORT);
}
