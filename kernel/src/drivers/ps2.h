#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct key_mod_t
{
    bool lshift : 1;
    bool rshift : 1;
    bool lctrl : 1;
    bool rctrl : 1;
    bool lalt : 1;
    bool ralt : 1;
    bool _reserved : 1;
} key_mod_t;

_Static_assert(sizeof(key_mod_t) == sizeof(uint8_t));

typedef struct key_event_t
{
    uint8_t   scancode;
    bool      released : 1;
    key_mod_t modifiers;
    char      ascii;
} key_event_t;

void        PS2_init(void);
uint8_t     PS2_get_scancode(void);
key_event_t PS2_get_char(void);
