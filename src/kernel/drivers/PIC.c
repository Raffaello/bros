#include <drivers/PIC.h>
#include <stdint.h>
#include <lib/io.h>


//=========================================================
//***              Controller Registers                 ***
//=========================================================
#define PIC1_REG_CMD        0x20    // Register Command
#define PIC1_REG_STATUS     0x20
#define PIC1_REG_DATA       0x21
#define PIC1_REG_IMR        0x21    // Interrupt Mask Register

#define PIC2_REG_CMD        0xA0
#define PIC2_REG_STATUS     0xA0
#define PIC2_REG_DATA       0xA1
#define PIC2_REG_IMR        0xA1

//=========================================================
//***         Initialization Control Words              ***
//=========================================================
#define PIC_ICW1_MASK_IC4   0x1     //00000001  // Expect ICW 4 bit
#define PIC_ICW1_MASK_SNGL  0x2     //00000010  // Single or Cascaded
#define PIC_ICW1_MASK_ADI   0x4     //00000100  // Call Address Interval
#define PIC_ICW1_MASK_LTIM  0x8     //00001000  // Operation Mode
#define PIC_ICW1_MASK_INIT  0x10    //00010000  // Initialization Command

#define PIC_ICW4_MASK_UPM   0x1     //00000001  // Mode
#define PIC_ICW4_MASK_AEOI  0x2     //00000010  // Automatic EOI
#define PIC_ICW4_MASK_MS    0x4     //00000100  // Selects buffer type
#define PIC_ICW4_MASK_BUF   0x8     //00001000  // Buffered mode
#define PIC_ICW4_MASK_SFNM  0x10    //00010000  // Special fully-nested mode

//=========================================================
//***    Initialization Command control words
//=========================================================
#define PIC_ICW1_IC4_EXPECT             1       // When 1, the PIC expects to recieve IC4 during initialization.
#define PIC_ICW1_IC4_NO                 0
#define PIC_ICW1_SNGL_YES               2       // When 1, only one PIC in system. If cleared, PIC is cascaded with slave PICs, and ICW3 must be sent to controller.
#define PIC_ICW1_SNGL_NO                0 
#define PIC_ICW1_ADI_CALLINTERVAL4      4       // When 1, CALL address interval is 4, else 8. Ignored by x86, and is default to 0
#define PIC_ICW1_ADI_CALLINTERVAL8      0
#define PIC_ICW1_LTIM_LEVELTRIGGERED    8       // When 1, CALL address interval is 4, else 8. Ignored by x86, and is default to 0
#define PIC_ICW1_LTIM_EDGETRIGGERED     0
#define PIC_ICW1_INIT_YES               0x10    // Initialization bit. Set 1 if PIC is to be initialized
#define PIC_ICW1_INIT_NO                0

#define PIC_ICW4_UPM_86MODE             1       // set 1 for 80x86 mode
#define PIC_ICW4_UPM_MCSMODE            0
#define PIC_ICW4_AEOI_AUTOEOI           2       // When 1, on the last interrupt ack, controller automatically performs End of Interrupt (EOI) operation
#define PIC_ICW4_AEOI_NOAUTOEOI         0
#define PIC_ICW4_MS_BUFFERMASTER        4       // use when BUF is set. When 1, selects buffer master. 0 for buffer slave.
#define PIC_ICW4_MS_BUFFERSLAVE         0
#define PIC_ICW4_BUF_MODEYES            8       // When 1, controller operates in buffered mode
#define PIC_ICW4_BUF_MODENO             0
#define PIC_ICW4_SFNM_NESTEDMODE        0x10    // Special Fully Nested Mode. Used in systems with a large amount of cascaded controllers.
#define PIC_ICW4_SFNM_NOTNESTED         0


// static inline void PIC_send_cmd(const uint8_t pic_reg_cmd, const uint8_t cmd)
// {
//     outb(pic_reg_cmd, cmd);
// }


// static inline void PIC_send_data(const uint8_t pic_reg_data, const uint8_t data)
// {
//     outb(pic_reg_data, data);
// }
 
// static inline uint8_t PIC_read_data(const uint8_t pic_reg_data)
// {
//     return inb(pic_reg_data);
// }

void PIC_init()
{
    uint8_t icw	= 0;

    // Begin initialization of PIC
    icw = (icw & ~PIC_ICW1_MASK_INIT) | PIC_ICW1_INIT_YES;
    icw = (icw & ~PIC_ICW1_MASK_IC4) | PIC_ICW1_IC4_EXPECT;
    outb(PIC1_REG_CMD, icw);
    outb(PIC2_REG_CMD, icw);

    // Send initialization control word 2. This is the base addresses of the irq's
    outb(PIC1_REG_DATA, PIC1_BASE_INT);
    outb(PIC2_REG_DATA, PIC2_BASE_INT);

    // Send initialization control word 3. This is the connection between master and slave.
    // ICW3 for master PIC is the IR that connects to secondary pic in binary format
    // ICW3 for secondary PIC is the IR that connects to master pic in decimal format
    outb(PIC1_REG_DATA, 0x04);
    outb(PIC2_REG_DATA, 0x02);

    // Send Initialization control word 4. Enables x86 mode
    icw = (icw & ~PIC_ICW4_MASK_UPM) | PIC_ICW4_UPM_86MODE;
    outb(PIC1_REG_DATA, icw);
    outb(PIC2_REG_DATA, icw);
}
