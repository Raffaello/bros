#pragma once

#define INT_Divide_Error                    0
#define INT_Debug_Exception                 1
#define INT_NMI_Interrupt                   2  // Non-Maskable-Interrupt
#define INT_Breakpoint                      3
#define INT_Overflow                        4
#define INT_Bbounds_Range_Exceeded          5
#define INT_Invalid_Opcode                  6
#define INT_Device_Not_Available            7  // (No Math CoProcessor)
#define INT_Double_Falt                     8
#define INT_Coprocessor_Segment_Overrun     9  // (Reserved)
#define INT_Invalid_TSS                     10 // Task Switch or TSS Access
#define INT_Segment_Not_Present             11
#define INT_Stack_Segment_Fault             12
#define INT_General_Protection              13
#define INT_Page_Fault                      14
#define INT_Intel_Reserved                  15 // Reserved
#define INT_FPU_Error                       16 // (Math Fault) x87 FPU floating-point or WAIT/FWAIT instruction.
#define INT_Alignment_Check                 17
#define INT_Machine_Check                   18
#define INT_SIMD_Floating_Point_Exception   19
#define INT_Virtualization_Exception        20
#define INT_Control_Protection_Exception    21
// INT_Intel_Reserved 22-31
// INT 32-255 User Defined Interrupts
// IRQ are mapped to INT 32-48
#define INT_RESERVED_START                  22
#define INT_RESERVED_END                    32+16 // IRQ

#define INT_TOTAL                           255 - (INT_RESERVED_END-INT_RESERVED_START)
