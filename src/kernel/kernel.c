#include <stdint.h>
#include <bios/vga.h>
#include <common/mem.h>

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

int a = add(1,2);
//  memset(&a, 1,0);

    clearVGA();

    for (int i=0; i<sizeof(hello_msg); i++)
    {
        writeVGAChar(20 + i, 10, hello_msg[i], 15);
    }

    writeVGAChar(0,0,a+'0',15);

    enable_cursor(0, 0);
    update_cursor(0, 24);

// Test div by zero, if not catch will reboot! :)
// need to disable some compiling switching for that
// Test for IDT
    // int a = 10/0; // div 0 error!:)
    while(1);
}
