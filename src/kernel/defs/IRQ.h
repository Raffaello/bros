#pragma once

// The following devices use PIC 1 to generate interrupts
#define IRQ_TIMER           0
#define IRQ_KEYBOARD        1
#define IRQ_CASCADE         2   // it will use IRQ 9 (9-8= 1 on PIC2)
#define IRQ_SERIAL2         3
#define IRQ_SERIAL1         4
#define IRQ_PARALLEL2       5
#define IRQ_DISKETTE        6
#define IRQ_PARALLEL1       7

// The following devices use PIC 2 to generate interrupts
#define IRQ_CMOSTIMER       0   // Real Time Clock (RTC)
#define IRQ_CGARETRACE      1   // ACPI
#define IRQ_OPEN1           2
#define IRQ_OPEN2           3
#define IRQ_AUX             4   // PS/2 Mouse
#define IRQ_FPU             5   // FPU or Inter-Processor Interrupt
#define IRQ_ATA_PRIMARY     6   // ATA Primary Controller
#define IRQ_ATA_SECONDARY   7   // ATA Secondary Controller 

#define IRQ_TOTAL           16