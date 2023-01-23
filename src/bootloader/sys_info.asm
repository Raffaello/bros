# System Info Data Structure
# to pass to the kernel

.ifndef __SYS_INFO__
__SYS_INFO:
  begin_marker: .long   0
  total_ram:    .long   0
  boot_device:  .byte   0
  end_marker:   .long   0
.endif