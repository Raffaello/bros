#include <drivers/fdc.h>
#include <sys/panic.h>
#include <arch/x86/io.h>
#include <arch/x86/ISR_IRQ.h>
#include <arch/x86/defs/IRQ.h>
#include <arch/x86/PIT.h>

#include <stdbool.h>

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
    //! just return main status register
    return inb(FDC_IO_MSR);
}

static void _fdc_send_command(uint8_t cmd)
{
    //! wait until data register is ready. We send commands to the data register
    for (int i = 0; i < 500; i++)
        if (_fdc_read_status() & FDC_MASK_MSR_DATAREG)
            return outb(FDC_IO_DR, cmd);
}

static uint8_t _fdc_read_data()
{
    //! same as above function but returns data register for reading
    for (int i = 0; i < 500; i++)
        if (_fdc_read_status() & FDC_MASK_MSR_DATAREG)
            return inb(FDC_IO_DR);

    return 0xFF;
}

static void _fdc_check_int(uint32_t* st0, uint32_t* cyl)
{
    _fdc_send_command(FDC_CMD_CHECK_INT);

    *st0 = _fdc_read_data();
    *cyl = _fdc_read_data();
}

//! wait for irq to fire
static inline void _fdc_wait_irq()
{
    //! wait for irq to fire
    while (!g_fdc_irq)
        ;
    g_fdc_irq = false;
}

static void _fdc_drive_data(uint32_t step_rate, uint32_t load_time, uint32_t unload_time, bool dma)
{
    uint32_t data = 0;

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
    sleep(20);
}

static bool _fdc_calibrate(uint8_t drive)
{
    uint32_t st0, cyl;

    if (drive >= 4)
        return false;

    // turn on the motor
    _fdc_control_motor(drive, true);
    for (int i = 0; i < 10; i++)
    {
        //! send command
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
    uint32_t st0, cyl;

    //! reset the controller
    outb(FDC_IO_DOR, 0);                                        // disable_controller
    outb(FDC_IO_DOR, FDC_MASK_DOR_RESET | FDC_MASK_DOR_DMA);    // enable_controller
    _fdc_wait_irq();

    //! send CHECK_INT/SENSE INTERRUPT command to all drives
    for (int i = 0; i < 4; i++)
        _fdc_check_int(&st0, &cyl);

    //! transfer speed 500kb/s
    outb(FDC_IO_CCR, 0);    // write_ccr(0);

    //! pass mechanical drive info. step-rate=3ms, unload time=240ms, load time=16ms
    _fdc_drive_data(3, 16, 240, true);

    //! calibrate the disk
    if (!_fdc_calibrate(drive))
        KERNEL_PANIC("unable to calibrate floppy drive: {}");
}

void fdc_init()
{
    IRQ_register_interrupt_handler(IRQ_DISKETTE, fdc_handler);

    _fdc_dma_init();
    _fdc_reset(0);    // TODO: get the number of floppies from CMOS ?
    _fdc_drive_data(13, 1, 0xF, true);
}
