#include <stdint.h>
#include <stdnoreturn.h>

__attribute__((section(".text._start")))
__attribute__((naked))
noreturn void  _start()
{
    // TODO remove the stack from the "kernel size",
    // wasting space on disk for nothing
    extern uint32_t __stack_end;
    extern void _start_entry();

    __asm__ volatile("mov esp, %0"::"i"(&__stack_end));
    __asm__ volatile("jmp %0"::"i"(&_start_entry));
}
