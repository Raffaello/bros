# Using BIOS Print to display a message to the user
# Using BIOS WaitKey before reboot
# Reboot the system

.ifndef Reboot

.include "bios/PrintString.asm"
.include "bios/WaitKey.asm"

.func Reboot
 Reboot:
  lea    si, rebootmsg # Load address of reboot message into si
  call   PrintString   # print the string
  call   WaitKey
  .byte  0xEA          # machine language to jump to FFFF:0000 (reboot)
  .word  0x0000
  .word  0xFFFF
.endfunc

rebootmsg: .asciz "Press any key to reboot.\r\n"
.endif
