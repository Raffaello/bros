#include <stdint.h>
#include <stdnoreturn.h>

__attribute__((section(".text.entry")))
__attribute__((naked))
noreturn void  entry()
{
    extern void _start();
    extern uint32_t __stack_end;
    extern uint32_t __stack_start;
    extern uint32_t __size;
    __asm__ volatile("mov esp, %0"::"i"(&__size));
    __asm__ volatile("mov esp, %0"::"i"(&__stack_start));
    __asm__ volatile("mov esp, %0"::"i"(&__stack_end));
    __asm__ volatile("jmp %0"::"i"(&_start));
}