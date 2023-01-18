#include <stdint.h>
#include <bios/vga.h>

void main();


// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
 __attribute__((force_align_arg_pointer))
void _start()
{
    // __asm__ ("sti");
    main();
}


void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    clearVGA();

    for (int i=0; i<sizeof(hello_msg); i++)
    {
        writeVGAChar(20+i, 10, hello_msg[i], 15);
    }

    enable_cursor(0, 1);
    update_cursor(1, 1);

    while(1);
}
