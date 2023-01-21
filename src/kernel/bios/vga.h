#pragma once

#include <stdint.h>

// TODO: review it, it is just basic, not done properly !!!
// TODO: get_cursor
// TODO: manage the cursor.
// TODO: change mode
// TODO: etc... stdio, stderr ...

void VGA_clear();
void VGA_WriteChar(const int x, const int y, const char ch, uint8_t col);
// TODO: optimize
void VGA_WriteString(const int x, const int y, const char str[], uint8_t col);

void VGA_enable_cursor(const uint8_t cursor_start, const uint8_t cursor_end);
void VGA_disable_cursor();

void VGA_update_cursor(const int x, const int y);
