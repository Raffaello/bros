/// @link: https://wiki.osdev.org/I8042_PS/2_Controller

#include <drivers/ps2.h>
#include <arch/x86/io.h>
#include <arch/x86/ISR_IRQ.h>
#include <arch/x86/defs/IRQ.h>
#include <lib/conio.h>
#include <sys/panic.h>

#include <stdbool.h>

#define PS2_DATA_PORT 0x60
#define PS2_STATUS    0x64
#define PS2_COMMAND   0x64

#define PS2_DATA_RESET                  0xFF
#define PS2_DATA_KEYBOARD_SCANCODE      0xF0
#define PS2_DATA_KEYBOARD_GET_SCANCODE  0x00
#define PS2_DATA_KEYBOARD_SET_SCANCODE1 0x01
#define PS2_DATA_KEYBOARD_SET_SCANCODE2 0x02
#define PS2_DATA_KEYBOARD_SET_SCANCODE3 0x03


#define PS2_RESPONSE_CTRL_TEST_OK 0x55
#define PS2_RESPONSE_ACK          0xFA
#define PS2_RESPONSE_SELF_TEST_OK 0xAA

// [[maybe_unused]] static char keyboard_scancode_set1[] = {
//     0x0,
//     0x1,
//     '1',
// };

static inline void _PS2_polling_wait_until_is_ready()
{
    while (true)
    {
        const uint8_t s = PS2_read_status();
        if ((s & 1) != 0)
            break;    // buffer not empty
    }
}

static inline void _PS2_polling_wait_until_can_send()
{
    while (true)
    {
        const uint8_t s = PS2_read_status();
        if ((s & 2) == 0)
            break;    // can write now, buffer empty
    }
}

static inline void _PS2_polling_send_command(uint8_t cmd)
{
    _PS2_polling_wait_until_can_send();
    PS2_send_command(cmd);
    // _PS2_polling_wait_until_is_ready();
}

static inline void _PS2_polling_send_data(uint8_t data)
{
    _PS2_polling_wait_until_can_send();
    outb(PS2_DATA_PORT, data);
}

static inline uint8_t _PS2_read_data(void)
{
    return inb(PS2_DATA_PORT);
}

static void _PS2_polling_send_ack(uint8_t data)
{
    _PS2_polling_send_data(data);
    if (_PS2_read_data() != PS2_RESPONSE_ACK)
        KERNEL_PANIC("PS/2 response not ACK");
}

static void _PS2_scancode_set(uint8_t set)
{
    _PS2_polling_send_ack(PS2_DATA_KEYBOARD_SCANCODE);
    _PS2_polling_send_ack(set);
}

static void _PS2_reset_device()
{
    uint8_t data, data2;

    _PS2_polling_send_data(PS2_DATA_RESET);
    _PS2_polling_wait_until_is_ready();
    data  = _PS2_read_data();
    data2 = _PS2_read_data();
    if (data != PS2_RESPONSE_ACK || data2 != PS2_RESPONSE_SELF_TEST_OK)
        KERNEL_PANIC("PS/2 PORT RESET FAIL");

    // 2 bytes device ID
    data  = _PS2_read_data();
    data2 = _PS2_read_data();
}

static void _keyboard_handler_scancode_set2(ISR_registers_t r)
{
    static uint8_t last_c = 0;
    uint8_t        c      = _PS2_read_data();
    r.eax                 = c;

    // if it is a released key...
    if (c == 0xF0 || last_c == 0xF0)
    {
        last_c = c;
        return;
    }

    switch (c)
    {
    case 0x16:
        CON_putc('1');
        break;
    case 0x1E:
        CON_putc('2');
        break;
    default:
        CON_putc(c);
    }

    last_c = c;
}

static void _mouse_handler(ISR_registers_t r)
{
    [[maybe_unused]] uint8_t c = _PS2_read_data();
}

void PS2_init(void)
{
    uint8_t data      = 0;
    uint8_t cfg       = 0;
    bool    has_port2 = false;

    // TODO: init USB controllers to avoid USB Legacy controllers emulating PS/2
    // TODO: check if PS/2 controller exists

    // Disable PS/2 ports
    _PS2_polling_send_command(PS2_COMMAND_DISABLE_PORT1);
    _PS2_polling_send_command(PS2_COMMAND_DISABLE_PORT2);
    // Flush Output Buffer
    _PS2_read_data();
    // Set controller Configuration Byte
    _PS2_polling_send_command(PS2_COMMAND_READ_CFG);
    cfg  = _PS2_read_data();
    cfg &= ~(1 << 0);    // First PS/2 port interrupt (1 = enabled, 0 = disabled)
    cfg &= ~(1 << 6);    // First PS/2 port translation (1 = enabled, 0 = disabled)
    cfg &= ~(1 << 4);    // First PS/2 port clock (1 = disabled, 0 = enabled)
    _PS2_polling_send_command(PS2_COMMAND_WRITE_CFG);
    _PS2_polling_send_data(cfg);
    // Self Test
    _PS2_polling_send_command(PS2_COMMAND_TEST);
    data = _PS2_read_data();
    if (data != PS2_RESPONSE_CTRL_TEST_OK)
        KERNEL_PANIC("PS/2 Controller TEST Failed");

    // Detect if there are 2 channels
    _PS2_polling_send_command(PS2_COMMAND_ENABLE_PORT2);
    _PS2_polling_send_command(PS2_COMMAND_READ_CFG);
    cfg = _PS2_read_data();
    if ((cfg & 32) == 0)
    {
        // it supports 2 ports
        has_port2 = true;
        _PS2_polling_send_command(PS2_COMMAND_DISABLE_PORT2);
        cfg &= ~(1 << 1);    // Second PS/2 port interrupt (1 = enabled, 0 = disabled, only if 2 PS/2 ports supported
        cfg &= ~(1 << 5);    // Second PS/2 port clock (1 = disabled, 0 = enabled, only if 2 PS/2 ports supported)
        _PS2_polling_send_command(PS2_COMMAND_WRITE_CFG);
        _PS2_polling_send_data(cfg);
    }

    // Interface Tests
    _PS2_polling_send_command(PS2_COMMAND_TEST_PORT1);
    data = _PS2_read_data();
    if (data != 0x00)
        KERNEL_PANIC("PS/2 PORT1 TEST Failed");

    if (has_port2)
    {
        _PS2_polling_send_command(PS2_COMMAND_TEST_PORT2);
        if (_PS2_read_data() != 0x00)
            KERNEL_PANIC("PS/2 PORT2 TEST Failed");
    }

    // set scancode set 2
    _PS2_scancode_set(PS2_DATA_KEYBOARD_SET_SCANCODE2);
    // get Scan code Set, expecting scan code set 2
    _PS2_scancode_set(PS2_DATA_KEYBOARD_GET_SCANCODE);
    data = _PS2_read_data();
    if (data != PS2_DATA_KEYBOARD_SET_SCANCODE2)
        KERNEL_PANIC("PS/2 Keyboard error");

    // Reset Devices
    _PS2_reset_device();
    if (has_port2)
    {
        _PS2_polling_send_command(PS2_COMMAND_WRITE_PORT2);
        _PS2_reset_device();
    }

    // Enable Devices
    _PS2_polling_send_command(PS2_COMMAND_ENABLE_PORT1);
    cfg |= (1 << 0);    // First PS/2 port interrupt (1 = enabled, 0 = disabled)
    if (has_port2)
    {
        _PS2_polling_send_command(PS2_COMMAND_ENABLE_PORT2);
        cfg |= (1 << 1);    // Second PS/2 port interrupt (1 = enabled, 0 = disabled, only if 2 PS/2 ports supported
    }

    _PS2_polling_send_command(PS2_COMMAND_WRITE_CFG);
    _PS2_polling_send_data(cfg);


    IRQ_register_interrupt_handler(IRQ_KEYBOARD, _keyboard_handler_scancode_set2);
    if (has_port2)
        IRQ_register_interrupt_handler(IRQ_AUX, _mouse_handler);
}

void PS2_send_command(uint8_t command)
{
    outb(PS2_COMMAND, command);
}

uint8_t PS2_read_status()
{
    return inb(PS2_STATUS);
}
