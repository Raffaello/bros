#pragma once

#include <stdint.h>

/* inline */ uint8_t  inb(const uint16_t port);
/* inline */ void     outb(const uint16_t port ,const uint8_t data);
/* inline */ uint16_t inw(const uint16_t port);
/* inline */ void     outw(const uint16_t port, const uint16_t data);
