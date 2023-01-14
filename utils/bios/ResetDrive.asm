# BIOS Reset Drive
# DL = expect drive unit (0=A:)

.ifndef ResetDrive
.func ResetDrive
ResetDrive:
  xor  ah, ah        # subfunction 0
  int  0x13          # call interrupt 13h
  ret
.endfunc
.endif
