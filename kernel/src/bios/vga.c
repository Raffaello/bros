/**
 * @link http://web.stanford.edu/class/cs140/projects/pintos/specs/freevga/vga/vga.htm 
**/

#include <bios/vga.h>
#include <lib/io.h>

#define VGA_MEM_TEXT 0xB8000

#define VGA_REG_CTRL 0x3D4
#define VGA_REG_DATA 0x3D5

#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 25

void VGA_fill(const uint8_t fg_col, const uint8_t bg_col)
{
    uint8_t *video_mem = (uint8_t*) VGA_MEM_TEXT;
    const register uint8_t v = (bg_col << 4) | fg_col;
    // TODO: using with eax=0 and rep movsd, shouldn't be faster?
    // TODO: besides this imply vga mode 3 :) ok for now
    for(int i = 0; i < VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2;) {
        video_mem[i++]=0;
        video_mem[i++]=v;
    }
}


void VGA_clear()
{
    // uint8_t *video_mem = (uint8_t*) 0xb8000;
    // // TODO: using with eax=0 and rep movsd, shouldn't be faster?
    // // TODO: besides this imply vga mode 3 :) ok for now
    // for(int i = 0; i < VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2;) {
    //     video_mem[i++]=0;
    //     video_mem[i++]=VGA_COLOR_GRAY;
    // }

    VGA_fill(VGA_COLOR_GRAY, VGA_COLOR_BLACK);
}

void VGA_WriteChar(const int x, const int y, const char ch, uint8_t col)
{
    uint8_t *video_mem = (uint8_t*) VGA_MEM_TEXT;
    const int off = (y * VGA_TEXT_WIDTH + x) * 2;
    video_mem[off] = ch;
    video_mem[off + 1] = col;
}

void VGA_WriteString(const int x, const int y, const char str[], uint8_t col)
{
    for(int i = 0;str[i] != 0; ++i)
    {
        // TODO: does it work if end of line? or end of screen? etc...
        VGA_WriteChar(x+i, y, str[i], col);
    }
}


void VGA_enable_cursor(const uint8_t cursor_start, const uint8_t cursor_end)
{
    outb(VGA_REG_CTRL, 0x0A);
    outb(VGA_REG_DATA, (inb(VGA_REG_DATA) & 0xC0) | cursor_start);
    outb(VGA_REG_CTRL, 0x0B);
    outb(VGA_REG_DATA, (inb(VGA_REG_DATA) & 0xE0) | cursor_end);
    outb(VGA_REG_DATA, 0xF);
}

void VGA_disable_cursor()
{
    outb(VGA_REG_CTRL, 0x0A);
    outb(VGA_REG_DATA, 0x20);
}

int VGA_get_cursor_offset()
{
    int offset;
    // reg 14: is the high byte of the cursor's offset
    // reg 15: is the low byte of the cursor's offset
    // Once the internal register has been selected , we may read or
    // write a byte on the data register .
    outb(VGA_REG_CTRL, 14);
    offset = inb(VGA_REG_DATA) << 8;
    outb(VGA_REG_CTRL, 15);
    offset += inb(VGA_REG_DATA);

    // Since the cursor offset reported by the VGA hardware is the
    // number of characters, it should be mul by two to convert it to
    // a character cell offset.
    return offset;
}

void VGA_update_cursor(const int x, const int y)
{
    const uint16_t pos = y * VGA_TEXT_WIDTH + x;

    outb(VGA_REG_CTRL, 0x0F);
    outb(VGA_REG_DATA, (uint8_t) (pos & 0xFF));
    outb(VGA_REG_CTRL, 0x0E);
    outb(VGA_REG_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

void VGA_scroll_down()
{
    uint8_t *video_mem = (uint8_t*) VGA_MEM_TEXT;
    register uint8_t col = video_mem[VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT) * 2 - 1];

    for(int i = 0; i < VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT-1) * 2 ;i++)
    {
        video_mem[i] = video_mem[i + (VGA_TEXT_WIDTH*2)];
    }

    for(int i = VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT-1) * 2; i < VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2;)
    {
        video_mem[i++]=0;
        video_mem[i++]=col; // last background char
    }
}
