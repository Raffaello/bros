#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <bios/vga.h>
#include <lib/stdlib.h>


noreturn void main()    __attribute__((section(".text.main")));


noreturn void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    // VGA_clear();
    VGA_WriteString(20, 10, hello_msg, 15);

    VGA_update_cursor(0, 24);

// Page Fault Test
    // intptr_t* t = (intptr_t*)0xFFFFFFFF;
    // *t=0;

    for(;;) {
        __asm__("hlt");
    }
}
