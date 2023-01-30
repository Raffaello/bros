// **************************************
// *** Console I/O                      *
// *** High-Level routines to deal with *
// *** VGA Text Mode 3                  *
// **************************************
#pragma once

#include <stdint.h>

#define CON_WIDTH 80
#define CON_HEIGHT 25

typedef struct con_col_t
{
    uint8_t fg_col:4;
    uint8_t bg_col:4;
} con_col_t;


void CON_setConsoleColor(const con_col_t col);
void CON_setConsoleColor2(const uint8_t bg_col, const uint8_t fg_col);
void CON_setBackgroundColor(const uint8_t bg_col);
void CON_setForegroundColor(const uint8_t fg_col);

void CON_gotoXY(const uint8_t x, const uint8_t y);
void CON_getXY(uint8_t* x, uint8_t* y);

void CON_putc(const char ch);
void CON_puts(const char str[]);
void CON_newline();

