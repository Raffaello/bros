#pragma once

#define PIC1_BASE_INT 0x20  // mapping interrupt at 0x20
#define PIC2_BASE_INT 0x28

// The following devices use PIC 1 to generate interrupts
#define PIC_IRQ_TIMER       0
#define PIC_IRQ_KEYBOARD    1
#define PIC_IRQ_SERIAL2     3
#define PIC_IRQ_SERIAL1     4
#define PIC_IRQ_PARALLEL2   5
#define PIC_IRQ_DISKETTE    6
#define PIC_IRQ_PARALLEL1   7

// The following devices use PIC 2 to generate interrupts
#define PIC_IRQ_CMOSTIMER   0
#define PIC_IRQ_CGARETRACE  1
#define PIC_IRQ_AUXILIARY   4
#define PIC_IRQ_FPU         5
#define PIC_IRQ_HDC         6


void PIC_init();
