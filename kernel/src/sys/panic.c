#include <sys/panic.h>
#include <bios/vga.h>
#include <lib/conio.h>

noreturn void panic(const char* err_msg, const char* filename, int line)
{
    __asm__("cli");

    //VGA_fill(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    CON_gotoXY(0, 0);
    CON_setConsoleColor2(VGA_COLOR_BLUE, VGA_COLOR_WHITE);
    CON_printf("Kernel Panic!\n%s\nfile: %s:%d",err_msg, filename, line);
    __asm__("hlt");
    for(;;);
}
