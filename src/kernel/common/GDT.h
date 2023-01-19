#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct GDT_descriptor_t
{
    uint16_t limit;
    uint16_t base_lo;
    uint8_t  base_mi;
    uint16_t flags;
    uint8_t  base_hi;
} GDT_descriptor_t;
_Static_assert(sizeof(GDT_descriptor_t) == 2+2+1+2+1);

typedef struct GDT_register_t
{
    uint16_t    size;
    uint32_t    base;
} GDT_register_t;
_Static_assert(sizeof(GDT_register_t) == 2+4);
#pragma pack(pop)

// static GDT_descriptor_t gdtd;
// static GDT_register_t   gdtr

void gdt_load(const GDT_descriptor_t* gdtd);
void gdt_set_descriptor();
