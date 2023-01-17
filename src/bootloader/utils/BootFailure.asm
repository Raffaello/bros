# Boot Failure code snippet.
# It will trigger reboot of the system.

.ifndef BootFailure

.include "bios/PrintStringNewLine.asm"
.include "utils/Reboot.asm"

.func BootFailure
BootFailure:
  call PrintStringNewLine
  call Reboot
.endfunc
.endif
