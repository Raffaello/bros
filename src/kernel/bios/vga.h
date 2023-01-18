#pragma once

#include <stdint.h>

void clearVGA();
void writeVGAChar(const int x, const int y, const char ch, uint8_t col);

void enable_cursor(const uint8_t cursor_start, const uint8_t cursor_end);
void disable_cursor();
void update_cursor(const int x, const int y);
