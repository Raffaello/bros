# Gate A20 Line functions

.ifndef __GATE_A20__
__GATE_A20__:

# Check if Gate A20 is enabled
# returns:
# return ZF=1 if enabled
.func CheckGateA20
CheckGateA20:
  push ds
  push es
  push di
  push si
  cli

  xor ax, ax                     # Set es:di = 0000:0500
  mov es, ax
  mov di, 0x0500

  not ax                         # Set ds:si = ffff:0510
  mov ds, ax
  mov si, 0x0510

  mov al, byte ptr es:[di]       # Save byte at es:di on stack.
  push ax

  mov al, byte ptr ds:[si]       # Save byte at ds:si on stack.
  push ax

  mov byte ptr es:[di], 0x00     # [es:di] = 0x00
  mov byte ptr ds:[si], 0xFF     # [ds:si] = 0xff

  cmp byte ptr es:[di], 0xFF     # Did memory wrap around?

  pop ax
  mov byte ptr ds:[si], al       # Restore byte at ds:si
  pop ax
  mov byte ptr es:[di], al       # Restore byte at es:di

  pop si
  pop di
  pop es
  pop ds

  ret
.endfunc
.endif
