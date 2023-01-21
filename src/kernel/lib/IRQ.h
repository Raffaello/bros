#pragma once

// The following devices use PIC 1 to generate interrupts
#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1
#define IRQ_SERIAL2     3
#define IRQ_SERIAL1     4
#define IRQ_PARALLEL2   5
#define IRQ_DISKETTE    6
#define IRQ_PARALLEL1   7

// The following devices use PIC 2 to generate interrupts
#define IRQ_CMOSTIMER   0
#define IRQ_CGARETRACE  1
#define IRQ_AUXILIARY   4
#define IRQ_FPU         5
#define IRQ_HDC         6
