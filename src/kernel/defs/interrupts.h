#pragma once

/*
Exception#  Description	Error                           Code?
-------------------------------------------------------------
0           Division By Zero Exception                  No
1           Debug Exception                             No
2           Non Maskable Interrupt Exception            No
3           Breakpoint Exception                        No
4           Into Detected Overflow Exception            No
5           Out of Bounds Exception                     No
6           Invalid Opcode Exception                    No
7           No Coprocessor Exception                    No
8           Double Fault Exception                      Yes
9           Coprocessor Segment Overrun Exception       No
10          Bad TSS Exception                           Yes
11          Segment Not Present Exception               Yes
12          Stack Fault Exception                       Yes
13          General Protection Fault Exception          Yes
14          Page Fault Exception                        Yes
15          Unknown Interrupt Exception                 No
16          Coprocessor Fault Exception                 No
17          Alignment Check Exception (486+)            No
18          Machine Check Exception (Pentium/586+)      No
19          to 31   Reserved Exceptions                 No
*/

#define INT_Divide_Error                    0
#define INT_Debug_Exception                 1
#define INT_NMI_Interrupt                   2  // Non-Maskable-Interrupt
#define INT_Breakpoint                      3
#define INT_Overflow                        4
#define INT_Bbounds_Range_Exceeded          5
#define INT_Invalid_Opcode                  6
#define INT_Device_Not_Available            7  // (No Math CoProcessor)
#define INT_Double_Fault                    8
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
// IRQ are mapped to INT 32-47

// #define INT_RESERVED_START                  22
// #define INT_RESERVED_END                    (32 + 15) // IRQ

// #define INT_RESERVED_TOTAL                  (INT_RESERVED_END - INT_RESERVED_START + 1)
// #define INT_TOTAL                           (255 - INT_RESERVED_TOTAL)
#define INT_TOTAL                           255
