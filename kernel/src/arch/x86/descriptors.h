/******************************
 *** Descriptor Tables      ***
 ***------------------------***
 *** 32 bits                ***
 *** GDT, IDT               ***
 ******************************/
#pragma once

#include <stdint.h>

/***
 * Data-Segment Descriptor (GDT)
 *   segment limit          0..15       word
 *   base address           16..31      word
 *   base 23:16             0..7        byte
 *   Accessed               8           
 *   Writable               9           
 *   Expansion direction    10         0=up 1=down
 *   zero                   11          
 *   one                    12          
 *   DPL                    13..14      0=kernel, 1,2 = drivers, 3=user app
 *   P                      15          Segment Present
 *   Limit 19:16            16..19      
 *   Available              20          
 *   reserved (zero)        21          
 *   Big                    22          0=SP 1=ESP, If expand-down data segment is the upper bound of the stack segment.
                                        If the flag is set, the upper bound is FFFFFFFFH (4 GBytes); otherwise FFFFH (64 KBytes). 
 *   Granularity            23          granularity=0, the segment range from 1 byte - 1 MByte, in byte increments.
 *                                      granularity=1, the segment range from 4 KBytes to 4 GBytes, in 4-KByte increments.
 *   Base 31:24             24..31      
 *
 * NOTES:
 * - An expand-down segment has maximum size when the segment limit is 0
 * - Only segment selectors of writable data segments can be loaded into the SS register.
 * */

/***
 * Code-Segment Descriptor (GDT)
 *   segment limit          0..15
 *   base address           16..31
 *   base 23:16             0..7
 *   Accessed               8
 *   Readble                9
 *   Conforming             10
 *   one                    11
 *   one                    12
 *   DPL                    13..14
 *   P                      15
 *   Limit 19:16            16..19
 *   Available              20
 *   reserved (zero)        21
 *   Default                22          1=32 protected mode, 0=16 bit protected mode
 *   Granularity            23
 *   Base 31:24             24..31
 * */

/***
 * System-Segment Descriptor (GDT)
 *   segment limit          0..15
 *   base address           16..31
 *   base 23:16             0..7
 *   Type                   8..11
 *   zero                   12
 *   DPL                    13..14
 *   P                      15
 *   Limit 19:16            16..19
 *   reserved               20
 *   reserved (zero)        21
 *   reserved               22
 *   Granularity            23
 *   Base 31:24             24..31
 * */

/**
 * Task-Segement Selector ?
 * Local-Segment Selector ?
 * 
 * NOTES:
 * - The LDTR can only be loaded with a selector for an LDT.
 * - The task register can only be loaded with a segment selector for a TSS.
 * 
 * - A far CALL or far JMP instruction can only access a segment descriptor for a conforming code segment,
 *   nonconforming code segment, call gate, task gate, or TSS.
 * - The LLDT instruction must reference a segment descriptor for an LDT.
 * - The LTR instruction must reference a segment descriptor for a TSS.
 * - The LAR instruction must reference a segment or gate descriptor for an LDT, TSS, call gate, task gate, code
 *   segment, or data segment.
 * - The LSL instruction must reference a segment descriptor for a LDT, TSS, code segment, or data segment.
 * - IDT entries must be interrupt, trap, or task gates.
 * 
 */

// typedef uint64_t GDT_descriptor_t;
typedef struct GDT_descriptor_t
{
    uint16_t limit;
    uint16_t base_lo;
    uint8_t  base_mi;
    uint8_t  attr;
    uint8_t  limit_attr;
    uint8_t  base_hi;
} __attribute((packed)) GDT_descriptor_t;
_Static_assert(sizeof(GDT_descriptor_t) == sizeof(uint64_t));

/**
 * used for GDT and IDT
 * 32 bits mode
 **/
typedef struct DT_register_t
{
    uint16_t    size;
    uint32_t    offset;
} __attribute__((packed)) DT_register_t;
_Static_assert(sizeof(DT_register_t) == 2+4);

typedef struct IDT_descriptor_t
{
    uint16_t    base_lo;
    uint16_t    selector;       // Segment Selector
    uint8_t     reserved;
    uint8_t     flags;          // Gate Type, DPL, P
    uint16_t    base_hi;

} __attribute__((packed)) IDT_descriptor_t;
_Static_assert(sizeof(IDT_descriptor_t) == 8);

// Interrupt handler function type definition
typedef void ((*IDT_Handler)(void));

void init_descriptor_tables();
void IDT_set_gate(const uint8_t numInt, IDT_Handler idt_func);
