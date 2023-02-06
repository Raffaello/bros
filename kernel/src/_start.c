#include <stdint.h>
#include <stdnoreturn.h>

__attribute__((section(".text._start")))
__attribute__((naked))
noreturn void  _start()
{
    // extern uint32_t __stack_end;
    extern void _start_entry();

    __asm__ ("cli");


    // TODO relocate the kernel to where? 
    //      need to have the memory map info before
    //      moving it around ... right?

    // TODO so the same is for setting up the stack.
    //      also the stack could benefit of its own selector


    // TODO move the minimum requirement here before jumping
    //      somewhere else.

    // __asm__ volatile("mov esp, %0"::"i"(&__stack_end));
    __asm__ volatile("jmp %0"::"i"(&_start_entry));
}
