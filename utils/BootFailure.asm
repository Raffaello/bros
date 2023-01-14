# Boot Failure code snippet.
# It will trigger reboot of the system.

.ifndef BootFailure

.include "utils/bios/PrintString.asm"
.include "utils/Reboot.asm"

.func BootFailure
BootFailure:
  call PrintString
  call Reboot
.endfunc
.endif
