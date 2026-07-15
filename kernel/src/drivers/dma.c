#include <drivers/dma.h>
#include <arch/x86/io.h>

#include <stdint.h>

#define DMA0_STATUS_REG             0x08
#define DMA0_COMMAND_REG            0x08
#define DMA0_REQUEST_REG            0x09
#define DMA0_CHANMASK_REG           0x0A
#define DMA0_MODE_REG               0x0B
#define DMA0_CLEARBYTE_FLIPFLOP_REG 0x0C
#define DMA0_TEMP_REG               0x0D
#define DMA0_MASTER_CLEAR_REG       0x0D
#define DMA0_CLEAR_MASK_REG         0x0E
#define DMA0_UNMASK_ALL_REG         0x0E
#define DMA0_MASK_REG               0x0F

#define DMA1_STATUS_REG             0xD0
#define DMA1_COMMAND_REG            0xD0
#define DMA1_REQUEST_REG            0xD2
#define DMA1_CHANMASK_REG           0xD4
#define DMA1_MODE_REG               0xD6
#define DMA1_CLEARBYTE_FLIPFLOP_REG 0xD8
#define DMA1_INTER_REG              0xDA
#define DMA1_UNMASK_ALL_REG         0xDC
#define DMA1_MASK_REG               0xDE

#define DMA0_CHAN0_ADDR_REG  0
#define DMA0_CHAN0_COUNT_REG 1
#define DMA0_CHAN1_ADDR_REG  2
#define DMA0_CHAN1_COUNT_REG 3
#define DMA0_CHAN2_ADDR_REG  4
#define DMA0_CHAN2_COUNT_REG 5
#define DMA0_CHAN3_ADDR_REG  6
#define DMA0_CHAN3_COUNT_REG 7

#define DMA1_CHAN4_ADDR_REG  0xC0
#define DMA1_CHAN4_COUNT_REG 0xC2
#define DMA1_CHAN5_ADDR_REG  0xC4
#define DMA1_CHAN5_COUNT_REG 0xC6
#define DMA1_CHAN6_ADDR_REG  0xC8
#define DMA1_CHAN6_COUNT_REG 0xCA
#define DMA1_CHAN7_ADDR_REG  0xCC
#define DMA1_CHAN7_COUNT_REG 0xCE

#define DMA_PAGE_EXTRA0          0x80
#define DMA_PAGE_CHAN2_ADDRBYTE2 0x81
#define DMA_PAGE_CHAN3_ADDRBYTE2 0x82
#define DMA_PAGE_CHAN1_ADDRBYTE2 0x83
#define DMA_PAGE_EXTRA1          0x84
#define DMA_PAGE_EXTRA2          0x85
#define DMA_PAGE_EXTRA3          0x86
#define DMA_PAGE_CHAN6_ADDRBYTE2 0x87
#define DMA_PAGE_CHAN7_ADDRBYTE2 0x88
#define DMA_PAGE_CHAN5_ADDRBYTE2 0x89
#define DMA_PAGE_EXTRA4          0x8C
#define DMA_PAGE_EXTRA5          0x8D
#define DMA_PAGE_EXTRA6          0x8E
#define DMA_PAGE_DRAM_REFRESH    0x8F

#define DMA_CMD_MASK_MEMTOMEM      1
#define DMA_CMD_MASK_CHAN0ADDRHOLD 2
#define DMA_CMD_MASK_ENABLE        4
#define DMA_CMD_MASK_TIMING        8
#define DMA_CMD_MASK_PRIORITY      0x10
#define DMA_CMD_MASK_WRITESEL      0x20
#define DMA_CMD_MASK_DREQ          0x40
#define DMA_CMD_MASK_DACK          0x8

#define DMA_MODE_MASK_SEL           3
#define DMA_MODE_MASK_TRA           0xc
#define DMA_MODE_SELF_TEST          0
#define DMA_MODE_READ_TRANSFER      4
#define DMA_MODE_WRITE_TRANSFER     8
#define DMA_MODE_MASK_AUTO          0x10
#define DMA_MODE_MASK_IDEC          0x20
#define DMA_MODE_MASK               0xc0
#define DMA_MODE_TRANSFER_ON_DEMAND 0
#define DMA_MODE_TRANSFER_SINGLE    0x40
#define DMA_MODE_TRANSFER_BLOCK     0x80
#define DMA_MODE_TRANSFER_CASCADE   0xC

///////////////////////////////////////////////////////////////////////////////

void dma_set_address(uint8_t channel, uint8_t low, uint8_t high)
{
    uint16_t port;
    switch (channel)
    {
    default:
        return;
    case 0:
        port = DMA0_CHAN0_ADDR_REG;
        break;
    case 1:
        port = DMA0_CHAN1_ADDR_REG;
        break;
    case 2:
        port = DMA0_CHAN2_ADDR_REG;
        break;
    case 3:
        port = DMA0_CHAN3_ADDR_REG;
        break;
    case 4:
        port = DMA1_CHAN4_ADDR_REG;
        break;
    case 5:
        port = DMA1_CHAN5_ADDR_REG;
        break;
    case 6:
        port = DMA1_CHAN6_ADDR_REG;
        break;
    case 7:
        port = DMA1_CHAN7_ADDR_REG;
        break;
    }

    outb(port, low);
    outb(port, high);
}

void dma_set_count(uint8_t channel, uint8_t low, uint8_t high)
{
    uint16_t port = 0;
    switch (channel)
    {
    default:
        return;
    case 0:
        port = DMA0_CHAN0_COUNT_REG;
        break;
    case 1:
        port = DMA0_CHAN1_COUNT_REG;
        break;
    case 2:
        port = DMA0_CHAN2_COUNT_REG;
        break;
    case 3:
        port = DMA0_CHAN3_COUNT_REG;
        break;
    case 4:
        port = DMA1_CHAN4_COUNT_REG;
        break;
    case 5:
        port = DMA1_CHAN5_COUNT_REG;
        break;
    case 6:
        port = DMA1_CHAN6_COUNT_REG;
        break;
    case 7:
        port = DMA1_CHAN7_COUNT_REG;
        break;
    }

    outb(port, low);
    outb(port, high);
}

void dma_set_external_page_register(uint8_t reg, uint8_t val)
{
    uint16_t port;
    switch (reg)
    {
    default:
        return;
    case 1:
        port = DMA_PAGE_CHAN1_ADDRBYTE2;
        break;
    case 2:
        port = DMA_PAGE_CHAN2_ADDRBYTE2;
        break;
    case 3:
        port = DMA_PAGE_CHAN3_ADDRBYTE2;
        break;
    case 4:
        return;    // nothing should ever write to register 4
    case 5:
        port = DMA_PAGE_CHAN5_ADDRBYTE2;
        break;
    case 6:
        port = DMA_PAGE_CHAN6_ADDRBYTE2;
        break;
    case 7:
        port = DMA_PAGE_CHAN7_ADDRBYTE2;
        break;
    }

    outb(port, val);
}

void dma_mask_channel(uint8_t channel)
{
    if (channel <= 4)
        outb(DMA0_CHANMASK_REG, (1 << (channel - 1)));
    else
        outb(DMA1_CHANMASK_REG, (1 << (channel - 5)));
}

void dma_unmask_channel(uint8_t channel)
{
    if (channel <= 4)
        outb(DMA0_CHANMASK_REG, channel);
    else
        outb(DMA1_CHANMASK_REG, channel);
}

void dma_set_mode(uint8_t channel, uint8_t mode)
{
    int dma  = (channel < 4) ? 0 : 1;
    int chan = (dma == 0) ? channel : channel - 4;

    dma_mask_channel(channel);
    outb((channel < 4) ? (DMA0_MODE_REG) : DMA1_MODE_REG, chan | (mode));
    dma_unmask_all(dma);
}

void dma_set_read(uint8_t channel)
{
    dma_set_mode(channel, DMA_MODE_READ_TRANSFER | DMA_MODE_TRANSFER_SINGLE | DMA_MODE_MASK_AUTO);
}

void dma_set_write(uint8_t channel)
{
    dma_set_mode(channel, DMA_MODE_WRITE_TRANSFER | DMA_MODE_TRANSFER_SINGLE | DMA_MODE_MASK_AUTO);
}

void dma_reset_flipflop(uint8_t dma)
{
    // if (dma > 2)
    //  return;

    //! it doesn't matter what is written to this register
    outb((dma == 0) ? DMA0_CLEARBYTE_FLIPFLOP_REG : DMA1_CLEARBYTE_FLIPFLOP_REG, 0xFF);
}

void dma_reset(uint8_t dma)
{
    outb((dma == 0) ? DMA0_CLEARBYTE_FLIPFLOP_REG : DMA1_CLEARBYTE_FLIPFLOP_REG, 0xFF);
}

void dma_unmask_all(uint8_t dma)
{
    outb((dma == 0) ? DMA0_UNMASK_ALL_REG : DMA1_UNMASK_ALL_REG, 0xFF);
}
