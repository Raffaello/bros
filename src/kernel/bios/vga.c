#include <bios/vga.h>
#include <drivers/io.h>

#define VGA_MEM_TEXT 0xb8000
#define VGA_REG_CTRL 0x3D4
#define VGA_REG_DATA 0x3D5

#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 25



void clearVGA()
{
    uint8_t *video_mem = (uint8_t*) 0xb8000;
    // TODO: using with eax=0 and rep movsd, shouldn't be faster?
    // TODO: besides this imply vga mode 3 :) ok for now
    for(int i = 0; i < VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2; i++) {
        video_mem[i]=0;
    }
}

void writeVGAChar(const int x, const int y, const char ch, uint8_t col)
{
    uint8_t *video_mem = (uint8_t*) VGA_MEM_TEXT;
    const int off = (y * VGA_TEXT_WIDTH + x) * 2;
    video_mem[off] = ch;
    video_mem[off + 1] = col;
}


void enable_cursor(const uint8_t cursor_start, const uint8_t cursor_end)
{
    outb(VGA_REG_CTRL, 0x0A);
    outb(VGA_REG_DATA, (inb(VGA_REG_DATA) & 0xC0) | cursor_start);
    outb(VGA_REG_CTRL, 0x0B);
    outb(VGA_REG_DATA, (inb(VGA_REG_DATA) & 0xE0) | cursor_end);
}

void disable_cursor()
{
    outb(VGA_REG_CTRL, 0x0A);
    outb(VGA_REG_DATA, 0x20);
}

void update_cursor(const int x, const int y)
{
    const uint16_t pos = y * VGA_TEXT_WIDTH + x;

    outb(VGA_REG_CTRL, 0x0F);
    outb(VGA_REG_DATA, (uint8_t) (pos & 0xFF));
    outb(VGA_REG_CTRL, 0x0E);
    outb(VGA_REG_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}