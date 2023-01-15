// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
__attribute__((force_align_arg_pointer))
void _start()
{
    int i = 0;
}

void main()
{
    char *video_mem = (char *)0xb8000; 
    *video_mem = 'X';
}
