#include <drivers/PIT.h>
#include <lib/IRQ.h>
#include <defs/IRQ.h>
#include <lib/io.h>
#include <lib/std.h>
#include <bios/vga.h>

#define PIT_FREQ    1193180

//-----------------------------------------------
//	Controller Registers
//-----------------------------------------------
#define PIT_REG_COUNTER0    0x40
#define PIT_REG_COUNTER1    0x41
#define PIT_REG_COUNTER2    0x42
#define PIT_REG_CMD         0x43

#define PIT_OCW_MASK_BINCOUNT       1       //00000001
#define PIT_OCW_MASK_MODE           0xE     //00001110
#define PIT_OCW_MASK_RL             0x30    //00110000
#define PIT_OCW_MASK_COUNTER        0xC0    //11000000

#define PIT_OCW_BINCOUNT_BINARY     0       //0
#define PIT_OCW_BINCOUNT_BCD        1       //1

#define PIT_OCW_MODE_TERMINALCOUNT  0       //0000
#define PIT_OCW_MODE_ONESHOT        0x2     //0010
#define PIT_OCW_MODE_RATEGEN        0x4     //0100
#define PIT_OCW_MODE_SQUAREWAVEGEN  0x6     //0110
#define PIT_OCW_MODE_SOFTWARETRIG   0x8     //1000
#define PIT_OCW_MODE_HARDWARETRIG   0xA     //1010

#define PIT_OCW_RL_LATCH            0       //000000
#define PIT_OCW_RL_LSBONLY          0x10    //010000
#define PIT_OCW_RL_MSBONLY          0x20    //100000
#define PIT_OCW_RL_DATA             0x30    //110000

#define PIT_OCW_COUNTER_0           0       //00000000
#define PIT_OCW_COUNTER_1           0x40    //01000000
#define PIT_OCW_COUNTER_2           0x80    //10000000

static uint32_t ticks = 0;

static void timer_callback(IRQ_registers_t r)
{
    char buf[11]; // 4294967295

    ticks++;
    itoa10(ticks, buf);
    VGA_WriteString(0,24, buf, VGA_COLOR_BRIGHT_GREEN);
}

void PIT_init()
{
    // Timer
    IRQ_register_interrupt_handler(IRQ_TIMER, timer_callback);
}

void PIT_set_timer_freq(const uint32_t freq)
{
    uint32_t divisor = PIT_FREQ / freq;

    // Send the command byte.
   outb(PIT_REG_CMD, PIT_OCW_BINCOUNT_BINARY | PIT_OCW_MODE_SQUAREWAVEGEN | PIT_OCW_RL_DATA | PIT_OCW_COUNTER_0);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   register uint8_t l = (uint8_t)(divisor & 0xFF);
   register uint8_t h = (uint8_t)( (divisor >> 8) & 0xFF);

   // Send the frequency divisor.
   outb(PIT_REG_COUNTER0, l);
   outb(PIT_REG_COUNTER0, h);
}
