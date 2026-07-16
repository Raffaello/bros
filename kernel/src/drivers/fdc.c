#include <drivers/fdc.h>
#include <drivers/dma.h>
#include <sys/panic.h>
#include <arch/x86/io.h>
#include <arch/x86/ISR_IRQ.h>
#include <arch/x86/defs/IRQ.h>
#include <arch/x86/PIT.h>
#include <lib/conio.h>
#include <defs/assert.h>

#include <stdbool.h>
#include <stddef.h>

#define FDC_RETRIES           500
#define FDC_SLEEP_TIME        5    // 20
#define FDC_SECTORS_PER_TRACK 18
#define FDC_DMA_CHANNEL       2
#define FDC_DMA_BUF_SIZE      512

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
#define FDC_CMD_READ_SECT 6
#define FDC_CMD_CALIBRATE 7
#define FDC_CMD_CHECK_INT 8
#define FDC_CMD_SEEK      0xF

#define FDC_CMD_EXT_SKIP       0x20
#define FDC_CMD_EXT_DENSITY    0x40
#define FDC_CMD_EXT_MULTITRACK 0x80

#define FDC_GAP3_LENGTH_3_5 27
#define FDC_SECTOR_DTL_512  2


static volatile bool g_fdc_irq = false;
static uint8_t*      g_pDmaBuf = (uint8_t*) 0x600;

static void fdc_handler(ISR_registers_t)
{
    g_fdc_irq = true;
}

static void _fdc_dma_init(uint8_t* buf, uint32_t length)
{
    const uint32_t b = (uint32_t) buf;
    const uint32_t l = length - 1;

    dma_reset(0);
    dma_mask_channel(FDC_DMA_CHANNEL);
    dma_reset_flipflop(0);
    dma_set_address(FDC_DMA_CHANNEL, b & 0xFF, (b >> 8) & 0xFF);
    dma_set_external_page_register(FDC_DMA_CHANNEL, (b >> 16) & 0xFF);
    dma_reset_flipflop(0);
    dma_set_count(FDC_DMA_CHANNEL, l & 0xFF, (l >> 8) & 0xFF);
    dma_set_read(FDC_DMA_CHANNEL);
    dma_unmask_channel(FDC_DMA_CHANNEL);
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

static bool _fdc_read_data(uint8_t* pData)
{
    if (pData == NULL)
        return false;

    for (int i = 0; i < FDC_RETRIES; i++)
    {
        if (_fdc_read_status() & FDC_MASK_MSR_DATAREG)
        {
            *pData = inb(FDC_IO_DR);
            return true;
        }
    }

    return false;
}

static void _fdc_check_int(uint8_t* st0, uint8_t* cyl)
{
    _fdc_send_command(FDC_CMD_CHECK_INT);

    if (!_fdc_read_data(st0) || !_fdc_read_data(cyl))
        KERNEL_PANIC("FDC unable to read data");    // TODO: should report error to the client
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

static bool _fdc_seek(uint8_t drive, uint8_t cyl, uint8_t head)
{
    uint8_t st0, cyl0;

    if (drive >= 4)
        return false;

    for (int i = 0; i < 10; i++)
    {
        _fdc_send_command(FDC_CMD_SEEK);
        _fdc_send_command((head) << 2 | drive);
        _fdc_send_command(cyl);
        _fdc_wait_irq();
        _fdc_check_int(&st0, &cyl0);
        // found the cylinder?
        if (cyl0 == cyl)
            return true;
    }

    return false;
}

static void _fdc_read_sector_imp(uint8_t drive, uint8_t head, uint8_t track, uint8_t sector)
{
    uint8_t st0, cyl;

    _fdc_dma_init((uint8_t*) g_pDmaBuf, FDC_DMA_BUF_SIZE);                                                     // initialize DMA
    dma_set_read(FDC_DMA_CHANNEL);                                                                             // set the DMA for read transfer
    _fdc_send_command(FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK | FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);    // read in a sector
    _fdc_send_command(head << 2 | drive);
    _fdc_send_command(track);
    _fdc_send_command(head);
    _fdc_send_command(sector);
    _fdc_send_command(FDC_SECTOR_DTL_512);
    _fdc_send_command(((sector + 1) >= FDC_SECTORS_PER_TRACK) ? FDC_SECTORS_PER_TRACK : sector + 1);
    _fdc_send_command(FDC_GAP3_LENGTH_3_5);
    _fdc_send_command(0xFF);
    _fdc_wait_irq();

    // read status info
    for (int j = 0; j < 7; j++)
        _fdc_read_data(&st0);

    _fdc_check_int(&st0, &cyl);    // let FDC know we handled interrupt
}

static void _fdc_lba_to_chs(int lba, int* pHead, int* pTrack, int* pSector)
{
    assert(pHead != NULL, "");
    assert(pTrack != NULL, "");
    assert(pSector != NULL, "");

    *pHead   = (lba % (FDC_SECTORS_PER_TRACK * 2)) / (FDC_SECTORS_PER_TRACK);
    *pTrack  = lba / (FDC_SECTORS_PER_TRACK * 2);
    *pSector = lba % FDC_SECTORS_PER_TRACK + 1;
}

///////////////////////////////////////////////////////////////////////////////

void fdc_init()
{
    IRQ_register_interrupt_handler(IRQ_DISKETTE, fdc_handler);

    // CMOS Floppy drive decode
    outb(0x70, 0x10);
    const uint8_t c            = inb(0x71);    // CMOS floppy drive encoded data
    const uint8_t a            = c >> 4;
    const uint8_t b            = c & 0xF;
    static char*  drive_type[] = {
        "None",
        "360KB  5.25in",
        "1.2MB  5.25in",
        "720KB  3.5in",
        "1.44MB 3.5in",
        "2.88MB 3.5in"};
    CON_printf("Floppy A: %s --- Floppy B: %s\n", drive_type[a], drive_type[b]);
    if (a != 0)
        _fdc_reset(0);
    if (b != 0)
        _fdc_reset(1);

    _fdc_drive_data(13, 1, 15, true);
}

void fdc_set_dma_addr(uint8_t* pBuf)
{
    g_pDmaBuf = pBuf;
}

uint8_t* fdc_read_sector(uint8_t drive, int sectorLBA)
{
    if (drive >= 4)
        return NULL;

    // convert LBA sector to CHS
    int head = 0, track = 0, sector = 1;
    _fdc_lba_to_chs(sectorLBA, &head, &track, &sector);

    // turn motor on and seek to track
    _fdc_control_motor(drive, true);
    if (!_fdc_seek(drive, (uint8_t) track, (uint8_t) head))
        return NULL;

    //! read sector and turn motor off
    _fdc_read_sector_imp(drive, head, (uint8_t) track, (uint8_t) sector);
    _fdc_control_motor(drive, false);

    // TODO: warning: this is a bit hackish
    return g_pDmaBuf;
}
