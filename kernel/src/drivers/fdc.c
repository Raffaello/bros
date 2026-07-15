#include <drivers/fdc.h>
#include <sys/panic.h>
#include <arch/x86/io.h>
#include <arch/x86/ISR_IRQ.h>
#include <arch/x86/defs/IRQ.h>
#include <arch/x86/PIT.h>
#include <lib/conio.h>

#include <stdbool.h>

#define FDC_RETRIES    500
#define FDC_SLEEP_TIME 20

#define FDC_IO_DOR 0x3F2
#define FDC_IO_MSR 0x3F4
#define FDC_IO_DR  0x3F5
#define FDC_IO_CCR 0xF7

#define FDC_MASK_DOR_RESET        4
#define FDC_MASK_DOR_DMA          8
#define FDC_MASK_DOR_DRIVE0_MOTOR 16
#define FDC_MASK_DOR_DRIVE1_MOTOR 32
#define FDC_MASK_DOR_DRIVE2_MOTOR 64
#define FDC_MASK_DOR_DRIVE3_MOTOR 128

#define FDC_MASK_MSR_DATAREG 128


#define FDC_CMD_SPECIFY   3
#define FDC_CMD_CALIBRATE 7
#define FDC_CMD_CHECK_INT 8

static volatile bool g_fdc_irq = false;

static void fdc_handler(ISR_registers_t)
{
    g_fdc_irq = true;
}

// TODO: evolve into a DMA file unit
static void _fdc_dma_init()
{
    outb(0x0A, 0x06);    // mask dma channel 2
    outb(0xD8, 0xFF);    // reset master flip-flop
    outb(0x04, 0);       // address=0x1000
    outb(0x04, 0x10);
    outb(0xD8, 0xFF);    // reset master flip-flop
    outb(0x05, 0xFF);    // count to 0x23ff (number of bytes in a 3.5" floppy disk track)
    outb(0x05, 0x23);
    outb(0x80, 0);       // external page register = 0
    outb(0x0A, 0x02);    // unmask dma channel 2
}

// TODO: evolve into a DMA file unit
[[maybe_unused]] static void _fdc_dma_read()
{
    outb(0x0A, 0x06);    // mask dma channel 2
    outb(0x0B, 0x56);    // single transfer, address increment, autoinit, read, channel 2
    outb(0x0A, 0x02);    // unmask dma channel 2
}

// TODO: evolve into a DMA file unit
[[maybe_unused]] static void _fdc_dma_write()
{
    outb(0x0A, 0x06);    // mask dma channel 2
    outb(0x0B, 0x5A);    // single transfer, address increment, autoinit, write, channel 2
    outb(0x0A, 0x02);    // unmask dma channel 2
}

static uint8_t _fdc_read_status()
{
    // just return main status register
    return inb(FDC_IO_MSR);
}

static void _fdc_send_command(uint8_t cmd)
{
    // wait until data register is ready. send commands to the data register
    for (int i = 0; i < FDC_RETRIES; i++)
    {
        if (_fdc_read_status() & FDC_MASK_MSR_DATAREG)
        {
            outb(FDC_IO_DR, cmd);
            return;
        }
    }
}

static uint8_t _fdc_read_data()
{
    for (int i = 0; i < FDC_RETRIES; i++)
    {
        if (_fdc_read_status() & FDC_MASK_MSR_DATAREG)
            return inb(FDC_IO_DR);
    }

    // TODO: review this function as it is conceptually wrong
    return 0xFF;    // TODO: THIS is an error, but it will be undetected!
}

static void _fdc_check_int(uint8_t* st0, uint8_t* cyl)
{
    _fdc_send_command(FDC_CMD_CHECK_INT);

    *st0 = _fdc_read_data();
    *cyl = _fdc_read_data();
}

static inline void _fdc_wait_irq()
{
    // wait for irq to fire
    while (!g_fdc_irq)
        ;

    g_fdc_irq = false;
}

/**
 * @brief This command is used to pass controlling information
 *        to the FDC about the mechanical drive connected to it.
 *
 * @param step_rate
 * @param load_time     Head Load Time
 * @param unload_time   Head Unload Time
 * @param dma           DMA Mode, use DMA?
 */
static void _fdc_drive_data(uint8_t step_rate, uint8_t load_time, uint8_t unload_time, bool dma)
{
    uint8_t data = 0;

    _fdc_send_command(FDC_CMD_SPECIFY);
    data = ((step_rate & 0xF) << 4) | (unload_time & 0xF);
    _fdc_send_command(data);
    data = (load_time) << 1 | (dma == true) ? 1 : 0;
    _fdc_send_command(data);
}

static void _fdc_control_motor(uint8_t drive, bool on)
{
    // turn on or off the motor of that drive
    if (on)
    {
        uint8_t motor;

        // select the correct mask based on current drive
        switch (drive)
        {
        case 0:
            motor = FDC_MASK_DOR_DRIVE0_MOTOR;
            break;
        case 1:
            motor = FDC_MASK_DOR_DRIVE1_MOTOR;
            break;
        case 2:
            motor = FDC_MASK_DOR_DRIVE2_MOTOR;
            break;
        case 3:
            motor = FDC_MASK_DOR_DRIVE3_MOTOR;
            break;
        default:
            // sanity check: invalid drive
            return;
        }

        outb(FDC_IO_DOR, drive | motor | FDC_MASK_DOR_RESET | FDC_MASK_DOR_DMA);
    }
    else
        outb(FDC_IO_DOR, FDC_MASK_DOR_RESET);

    // in all cases; wait a little bit for the motor to spin up/turn off
    sleep(FDC_SLEEP_TIME);
}

static bool _fdc_calibrate(uint8_t drive)
{
    uint8_t st0, cyl;

    if (drive >= 4)
        return false;

    // turn on the motor
    _fdc_control_motor(drive, true);
    for (int i = 0; i < 10; i++)
    {
        // send command
        _fdc_send_command(FDC_CMD_CALIBRATE);
        _fdc_send_command(drive);
        _fdc_wait_irq();
        _fdc_check_int(&st0, &cyl);
        // did we fine cylinder 0? if so, we are done
        if (!cyl)
        {
            _fdc_control_motor(drive, false);
            return true;
        }
    }

    _fdc_control_motor(drive, false);
    return false;
}

static void _fdc_reset(uint8_t drive)
{
    uint8_t st0, cyl;

    // reset the controller
    outb(FDC_IO_DOR, 0);                                        // disable_controller
    outb(FDC_IO_DOR, FDC_MASK_DOR_RESET | FDC_MASK_DOR_DMA);    // enable_controller
    _fdc_wait_irq();

    // send CHECK_INT/SENSE INTERRUPT command to all drives
    for (int i = 0; i < 4; i++)
        _fdc_check_int(&st0, &cyl);

    // transfer speed 500kb/s
    outb(FDC_IO_CCR, 0);    // write_ccr(0);

    // pass mechanical drive info. step-rate=3ms, unload time=240ms, load time=16ms
    _fdc_drive_data(3, 16, 240, true);

    // calibrate the disk
    if (!_fdc_calibrate(drive))
        KERNEL_PANIC("unable to calibrate floppy drive: {}");
}

void fdc_init()
{
    IRQ_register_interrupt_handler(IRQ_DISKETTE, fdc_handler);

    _fdc_dma_init();    // TODO: init DMA separately?

    // CMOS Floppy drive decode
    outb(0x70, 0x10);
    const uint8_t c            = inb(0x71);    // CMOS floppy drive encoded data
    const uint8_t a            = c >> 4;
    const uint8_t b            = c & 0xF;
    static char*  drive_type[] = {
        "no floppy drive",
        "360kb 5.25in floppy drive",
        "1.2mb 5.25in floppy drive",
        "720kb 3.5in",
        "1.44mb 3.5in",
        "2.88mb 3.5in"};
    CON_printf("Floppy A: %s --- Floppy B: %s\n", drive_type[a], drive_type[b]);
    if (a != 0)
        _fdc_reset(0);
    if (b != 0)
        _fdc_reset(1);

    _fdc_drive_data(13, 1, 15, true);
}
