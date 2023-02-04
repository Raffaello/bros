# Boot Failure code snippet.
# It will trigger reboot of the system.

.ifndef BootFailure

.include "bios/PrintStringNewLine.inc"
.include "utils/Reboot.inc"

.func BootFailure
BootFailure:
  call PrintStringNewLine
  call Reboot
.endfunc
.endif
