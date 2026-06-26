/// @link: https://wiki.osdev.org/I8042_PS/2_Controller

#include <drivers/ps2.h>
#include <arch/x86/io.h>
#include <arch/x86/ISR_IRQ.h>
#include <arch/x86/defs/IRQ.h>
#include <lib/conio.h>
#include <sys/panic.h>

#include <stdbool.h>
#include <stddef.h>

#define PS2_DATA_PORT 0x60
#define PS2_STATUS    0x64
#define PS2_COMMAND   0x64

#define PS2_DATA_RESET                  0xFF
#define PS2_DATA_KEYBOARD_SCANCODE      0xF0
#define PS2_DATA_KEYBOARD_GET_SCANCODE  0x00
#define PS2_DATA_KEYBOARD_SET_SCANCODE1 0x01
#define PS2_DATA_KEYBOARD_SET_SCANCODE2 0x02
#define PS2_DATA_KEYBOARD_SET_SCANCODE3 0x03
#define PS2_DATA_MOUSE_SET_DEFAULTS     0xFA
#define PS2_DATA_MOUSE_ENABLE_DATA_REP  0xF4


#define PS2_RESPONSE_CTRL_TEST_OK 0x55
#define PS2_RESPONSE_ACK          0xFA
#define PS2_RESPONSE_SELF_TEST_OK 0xAA

///////////////////////////////////////////////////////////////////////////////

// TODO: replace with a generic ring_buffer, when have dynamic memory or kalloc or something.
#define PS2_RING_BUFFER_SIZE 16    // 256
_Static_assert(PS2_RING_BUFFER_SIZE <= 256);

typedef struct ring_buf_t
{
    uint8_t          buf[PS2_RING_BUFFER_SIZE];
    volatile uint8_t head;    // this will overflow, so no need to module PS2_RING_BUFFER_SIZE
    volatile uint8_t tail;
} ring_buf_t;

static ring_buf_t g_ring_buf;

static bool _PS2_ring_buf_empty()
{
    return g_ring_buf.head == g_ring_buf.tail;
}

static bool _PS2_ring_buf_full()
{
    return (g_ring_buf.head + 1) % PS2_RING_BUFFER_SIZE == g_ring_buf.tail;
}

static void _PS2_ring_buf_push(uint8_t scancode)
{
    // if it is full
    if (_PS2_ring_buf_full())
        return;

    g_ring_buf.buf[g_ring_buf.head] = scancode;
    g_ring_buf.head                 = (g_ring_buf.head + 1) % PS2_RING_BUFFER_SIZE;
}

static bool _PS2_ring_buf_pop(uint8_t* pScancode)
{
    if (pScancode == NULL)
        return false;

    if (_PS2_ring_buf_empty())
        return false;

    *pScancode      = g_ring_buf.buf[g_ring_buf.tail];
    g_ring_buf.tail = (g_ring_buf.tail + 1) % PS2_RING_BUFFER_SIZE;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

static void _PS2_send_command(uint8_t command)
{
    outb(PS2_COMMAND, command);
}

static uint8_t _PS2_read_status()
{
    return inb(PS2_STATUS);
}

static inline void _PS2_polling_wait_until_is_ready()
{
    while (true)
    {
        const uint8_t s = _PS2_read_status();
        if ((s & 1) != 0)
            break;    // buffer not empty
    }
}

static inline void _PS2_polling_wait_until_can_send()
{
    while (true)
    {
        const uint8_t s = _PS2_read_status();
        if ((s & 2) == 0)
            break;    // can write now, buffer empty
    }
}

static inline void _PS2_polling_send_command(uint8_t cmd)
{
    _PS2_polling_wait_until_can_send();
    _PS2_send_command(cmd);
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
    _PS2_polling_wait_until_is_ready();
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
    uint8_t data;

    _PS2_polling_send_ack(PS2_DATA_RESET);
    data = _PS2_read_data();
    if (data != PS2_RESPONSE_SELF_TEST_OK)
        KERNEL_PANIC("PS/2 PORT RESET FAIL");

    // 2 bytes device ID
    data = _PS2_read_data();
    data = _PS2_read_data();
}

static void _keyboard_handler_scancode(ISR_registers_t r)
{
    _PS2_polling_wait_until_is_ready();
    const uint8_t c = _PS2_read_data();
    _PS2_ring_buf_push(c);
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

        // TODO: PS/2 MOUSE has some troubles in QEMU?

        // _PS2_polling_send_command(PS2_COMMAND_WRITE_PORT2);
        // _PS2_polling_send_ack(PS2_DATA_MOUSE_SET_DEFAULTS);
        // _PS2_polling_send_command(PS2_COMMAND_WRITE_PORT2);
        // _PS2_polling_send_ack(PS2_DATA_MOUSE_ENABLE_DATA_REP);
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


    IRQ_register_interrupt_handler(IRQ_KEYBOARD, _keyboard_handler_scancode);
    if (has_port2)
        IRQ_register_interrupt_handler(IRQ_AUX, _mouse_handler);
}

uint8_t PS2_get_scancode(void)
{
    uint8_t scancode = 0;

    while (!_PS2_ring_buf_pop(&scancode))
    {
        while (_PS2_ring_buf_empty())
        {
            // TODO: wait...
        }
    }

    return scancode;
}

key_event_t PS2_get_char(void)
{
    key_event_t k        = {};
    bool        extended = false;

    while (k.scancode == 0)
    {
        uint8_t c = PS2_get_scancode();
        // if (c == 0)    // TODO: remove this is for testing
        //     break;

        if (c == 0xE0)    // if it is an extended key...
        {
            extended = true;
            continue;
        }

        if (c == 0xF0)    // if it is a released key...
        {
            k.released = true;
            continue;
        }

        if (extended)
        {
            switch (c)
            {
            case 0x14:    // R_CTRL (0xE0, 0x14)
                k.modifiers.rctrl = true;
                break;

            case 0x11:    // R_ALT (0xE0, 0x11)
                k.modifiers.ralt = true;
                break;

            default:
                k.scancode = c;
                break;
            }
        }
        else
        {
            switch (c)
            {
            case 0xE0:    // if it is an extended key..
                extended = true;
                break;

            case 0x12:    // L_SHIFT
                k.modifiers.lshift = true;
                break;

            case 0x59:    // R_SHIFT
                k.modifiers.rshift = true;
                break;

            case 0x14:    // L_CTRL
                k.modifiers.lctrl = true;
                break;

            case 0x11:    // L_ALT
                k.modifiers.lalt = true;
                break;

            default:
                k.scancode = c;
                break;
            }
        }
    }

    // TODO: populate ASCII value
    // TODO: review the modifiers
    // k.ascii = 0;
    if (!k.released)
    {
        switch (k.scancode)
        {
        default:
            break;
        case 0x16:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '!';
            else
                k.ascii = '1';
            break;
        case 0x1E:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '@';
            else
                k.ascii = '2';
            break;
        case 0x26:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '#';
            else
                k.ascii = '3';
            break;
        case 0x25:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '$';
            else
                k.ascii = '4';
            break;
        case 0x2E:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '%';
            else
                k.ascii = '5';
            break;
        case 0x36:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '^';
            else
                k.ascii = '6';
            break;
        case 0x3D:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '&';
            else
                k.ascii = '7';
            break;
        case 0x3E:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '*';
            else
                k.ascii = '8';
            break;
        case 0x46:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = '(';
            else
                k.ascii = '9';
            break;
        case 0x45:
            if (k.modifiers.lshift || k.modifiers.rshift)
                k.ascii = ')';
            else
                k.ascii = '0';
            break;
        }
    }

    return k;
}
