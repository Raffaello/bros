void main();

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
__attribute__((force_align_arg_pointer))
void _start()
{
    main();
}

void main()
{
    char *video_mem = (char *)0xb8000; 
    *video_mem = 'X';
    while(1);
}
