#pragma once

#include <stdint.h>

#pragma pack(push, 1)

/**
 * used for both GDT and IDT
 * 32 bits mode
 **/
typedef struct DT_register_t
{
    uint16_t    size;
    uint32_t    offset;
} __attribute__((packed)) DT_register_t;
_Static_assert(sizeof(DT_register_t) == 2+4);

typedef struct GDT_descriptor_t
{
    uint16_t    limit;
    uint16_t    base_lo;
    uint8_t     base_mi;
    uint8_t     access;
    uint8_t     limit_flags;
    uint8_t     base_hi;
} __attribute__((packed)) GDT_descriptor_t;
_Static_assert(sizeof(GDT_descriptor_t) == 8);

typedef struct IDT_descriptor_t
{
    uint16_t    base_lo;
    uint16_t    selector;       // Segment Selector
    uint8_t     reserved;
    uint8_t     flags;          // Gate Type, DPL, P
    uint16_t    base_hi;

} __attribute__((packed)) IDT_descriptor_t;
_Static_assert(sizeof(IDT_descriptor_t) == 8);

#pragma pack(pop)

void gdt_load(const DT_register_t* dtr);
void idt_load(const DT_register_t* dtr);

void gdt_initialize();
void idt_initialize(/*uint16_t codeSel*/);

void idt_default_handler();
