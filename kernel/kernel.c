#include <stdint.h>

void main();

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
 __attribute__((force_align_arg_pointer))
void _start()
{
    main();
}

void clearVGA()
{
    uint8_t *video_mem = (uint8_t *)0xb8000;
    for(int i = 0; i< 80*25*2;i++){
        video_mem[i]=0;
    }
}

void writeVGAChar(const int x, const int y, const char ch, uint8_t col)
{
    uint8_t *video_mem = (uint8_t *)0xb8000;
    const int off = (y*80 + x) * 2;

    video_mem[off] = ch;
    video_mem[off+1] = col;
}



void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    // writeVGAChar(1, 2, 'A', '7');
    // writeVGAChar(0,1, 'Z', 15);
    // writeVGAChar(10,10, 'W', 15);

    clearVGA();

    for (int i=0; i<sizeof(hello_msg); i++)
    {
        writeVGAChar(20+i, 10, hello_msg[i], 15);
    }

    while(1);
}
