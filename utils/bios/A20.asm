# # A20 Line

# .ifndef EnableA20Line
# .func EnableA20Line
# EnableA20Line:
#   push ds
#   push di
#   push si

#   mov ax, 0xFFFF
#   mov ds, ax

#   mov di, 0x0500
#   mov si, 0x0510

#   mov al, byte [es:di]
#   push ax
#   mov al, byte [ds:si]
#   push ax

#   mov byte [es:di], 0x00
#   mov byte [ds:si], 0xFF

#   cmp byte [es:di], 0xFF

#   pop ax
#   mov byte [ds:si], al

#   pop ax
#   mov byte [es:di], al

#   mov ax, 0
#   je check_a20__exit
#   mov ax, 1

#   pop si
#   pop di
#   pop ds
#   ret
# .endfunc
# .endif
