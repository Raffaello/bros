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


// TODO: fix bootloader to load kernel
//    , as it is not loading when more then 1 sector apparently.
void main()
{
    const char hello_msg[] = "***1 HELLO FROM BROSKRNL.SYS ***";
    const char hello_msg1[] = "***2 HELLO FROM BROSKRNL.SYS ***";

    clearVGA();
    // enable_cursor(0, 1);
    // update_cursor(0, 0);

    for (int i=0; i<sizeof(hello_msg); i++)
    {
        writeVGAChar(20+i, 10, hello_msg[i], 15);
    }

    // enable_cursor(0, 0);
    // update_cursor(1, 1);
    

    while(1);
}
