# Boot Failure code snippet.
# It will trigger reboot of the system.

.ifndef "PrintString"
.include "utils/bios/PrintString.asm"
.endif
.ifndef "Reboot"
.include "utils/Reboot.asm"
.endif

.func BootFailure
BootFailure:
  call PrintString
  call Reboot
.endfunc
