#include <lib/conio.h>
#include <bios/vga.h>
#include <lib/string.h>
#include <lib/std.h>
#include <stdarg.h>

static uint8_t _col = 7;
static uint8_t _curX = 0;
static uint8_t _curY = 0;

static void _AdjscrollDrown()
{
    if(_curX >= CON_WIDTH) {
        _curX = 0;
        _curY++;
    }

    if(_curY >= CON_HEIGHT) {
        VGA_scroll_down();
        _curY--;
    }

    VGA_update_cursor(_curX, _curY);
}

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

con_col_t CON_getConsoleColor()
{
    return (con_col_t) { .bg_col = _col >> 4, .fg_col = _col & 0xF };
}

uint8_t   CON_getConsoleColor2()
{
    return _col;
}

void CON_gotoXY(const uint8_t x, const uint8_t y)
{
    VGA_update_cursor(x, y);
    _curX = x;
    _curY = y;
}

void CON_getXY(uint8_t* x, uint8_t* y)
{
    // TODO should call a VGA function to get the cursor position, instead of using cached values?
    *x = _curX;
    *y = _curY;
}

void CON_putc(const char ch)
{
    switch(ch)
    {
    case '\n':
        CON_newline();
    break;
    default:
        VGA_WriteChar(_curX, _curY, ch, _col);
        _curX++;
        _AdjscrollDrown();
    break;
    }
}

void CON_puts(const char str[])
{
    for(int i = 0; str[i] != 0; i++)
        CON_putc(str[i]);
}

void CON_newline()
{
    CON_gotoXY(0, _curY + 1);
    _AdjscrollDrown();
}

void CON_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    for (; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            CON_putc(*fmt);
            continue;
        }

        ++fmt;

        switch (*fmt)
        {
            case '%':
            {
                CON_putc('%');
                continue;
            }
            /*** characters ***/
            case 'c':
            {
                char c = (char) va_arg (args, int);
                CON_putc(c);
                continue;
            }
            /*** string ***/
            case 's':
            {
                const char* s = va_arg (args, char*);
                CON_puts(s);
                continue;
            }
            /*** integers ***/
            
            case 'd':
            case 'i':
            // {
            //     // TODO not supported, not able to print negative numbers
            //     int i = va_arg (args, int);
            //     char buf[12];
            //     CON_puts(itoa10(i, buf));
            //     continue;
            // }
            case 'u':
            {
                unsigned u = va_arg (args, unsigned int);
                char buf[12];
                CON_puts(itoa(u, buf, 10));
                continue;
            }
            /*** display in hex ***/
            case 'X':
            case 'x':
            {
                int i = va_arg (args, int);
                char buf[12];
                CON_puts(itoa(i, buf, 16));
                continue;
            }
            default:
            {
                CON_putc('%');
                CON_putc(*fmt);
                continue;
            }
        }
    }

    va_end (args);
}
