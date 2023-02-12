#include <sys/panic.h>
#include <bios/vga.h>

noreturn void kernel_panic(const char* err_msg)
{
    VGA_fill(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    VGA_WriteString(0,0, "Kernel Panic!", VGA_COLOR_WHITE);
    VGA_WriteString(0,2, err_msg, VGA_COLOR_WHITE);
    __asm__("hlt");
    for(;;);
}
