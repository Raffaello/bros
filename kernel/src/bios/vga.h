#pragma once

#include <stdint.h>

// TODO: review it, it is just basic, not done properly !!!
// TODO: change mode
// TODO: etc... stdio, stderr ...


#define VGA_COLOR_BLACK          0
#define VGA_COLOR_BLUE           1
#define VGA_COLOR_GREEN          2
#define VGA_COLOR_CYAN           3
#define VGA_COLOR_RED            4
#define VGA_COLOR_MAGENTA        5
#define VGA_COLOR_BROWN          6
#define VGA_COLOR_GRAY           7
#define VGA_COLOR_DARK_GRAY      8
#define VGA_COLOR_BRIGHT_BLUE    9
#define VGA_COLOR_BRIGHT_GREEN   10
#define VGA_COLOR_BRIGHT_CYAN    11
#define VGA_COLOR_BRIGHT_RED     12
#define VGA_COLOR_BRIGHT_MAGENTA 13
#define VGA_COLOR_YELLOW         14
#define VGA_COLOR_WHITE          15

#define VGA_TEXT_MODE_3_COLS 80
#define VGA_TEXT_MODE_3_ROWS 25

// #define VGA_GET_X_FROM_OFFSET(offs) offs % 80
// #define VGA_GET_Y_FROM_OFFSET(offs) offs / 80

typedef struct VGA_char_t
{
    char    ch;
    uint8_t col;
} __attribute__((packed)) VGA_char_t;

_Static_assert(sizeof(VGA_char_t) == sizeof(uint16_t));


void VGA_fill(const uint8_t fg_col, const uint8_t bg_col);
void VGA_clear();

void VGA_WriteChar(const int x, const int y, const char ch, uint8_t col);
void VGA_Write(const int x, const int y, VGA_char_t* ch);
void VGA_WriteString(const int x, const int y, const char str[], uint8_t col);

/**
 * @return char | col << 8
 */
uint16_t   VGA_GetChar(const int x, const int y);
VGA_char_t VGA_Read(const int x, const int y);

void VGA_enable_cursor(const uint8_t cursor_start, const uint8_t cursor_end);
void VGA_disable_cursor();

// cursor offset, linear x,y coordinate, must be multiplied by 2 for VGA offset
int  VGA_get_cursor_offset();
void VGA_update_cursor(const int x, const int y);

void VGA_scroll_down();
