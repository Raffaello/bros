# BIOS WaitKey
.ifndef WaitKey
.func WaitKey
WaitKey:
  xor    ax, ax # subfunction 0
  int    0x16   # call bios to wait for key
  ret
.endfunc
.endif
