#pragma once

#define INT_Divide_Error                    00 
#define INT_Debug_Exception                 01
#define INT_NMI_Interrupt                   02 // Non-Maskable-Interrupt
#define INT_Breakpoint                      03
#define INT_Overflow                        04
#define INT_Bbounds_Range_Exceeded          05
#define INT_Invalid_Opcode                  06
#define INT_Device_Not_Available            07 // (No Math CoProcessor)
#define INT_Double_Falt                     08
#define INT_Coprocessor_Segment_Overrun     09 // (Reserved)
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
