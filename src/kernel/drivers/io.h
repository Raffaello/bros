#pragma once

#include <stdint.h>

/*static inline*/ uint8_t  inb(const uint16_t port);
/*static inline*/ void     outb(const uint16_t port ,const uint8_t data);
/*static inline*/ uint16_t inw(const uint16_t port);
/*static inline*/ void     outw(const uint16_t port, const uint16_t data);

