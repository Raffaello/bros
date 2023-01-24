#include <lib/conio.h>
#include <bios/vga.h>

static uint8_t _col;
static uint8_t _curX = 0;
static uint8_t _curY = 0;


void CON_setConsoleColor(const con_col_t col)
{
    CON_setConsoleColor2(col.bg_col, col.fg_col);
}

void CON_setConsoleColor2(const uint8_t bg_col, const uint8_t fg_col)
{
    _col = (bg_col << 4) | (fg_col & 0xF);
}

void CON_setBackgroundColor(const uint8_t bg_col)
{
    _col |= (bg_col << 4);
}
void CON_setForegroundColor(const uint8_t fg_col)
{
    _col |= (fg_col & 0xF);
}

void CON_gotoXY(const uint8_t x, const uint8_t y)
{
    VGA_update_cursor(x, y);
    _curX = x;
    _curY = y;
}

void CON_getXY(uint8_t* x, uint8_t* y)
{
    // TODO should call a VGA function to get the cursor position, instead of using cached values
    *x = _curX;
    *y = _curY;
}

void CON_putc(const char ch)
{
    VGA_WriteChar(_curX, _curY, ch, _col);
    _curX++;
    if(_curX >= CON_WIDTH) {
        _curX =0;
        _curY++;
    }

    if(_curY >= CON_HEIGHT) {
        VGA_scroll_down();
        _curY--;
    }

    VGA_update_cursor(_curX, _curY);
}

void CON_puts(const char str[]);

