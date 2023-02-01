/* -*-Asm-*- */

# Interrupt Request Routines
# IRQ are defined in defs/IRQ.h

.code32
.intel_syntax noprefix
.text

.macro IRQ_INT x
.globl IRQ_INT_\x
IRQ_INT_\x:
  cli
  push 0    # this can be used instead for the re-maped interrupt value
  push \x
  jmp IRQ_universal_handler_caller
.type IRQ_INT_\x, @function
.size IRQ_INT_\x, .-IRQ_INT_\x
.endm

IRQ_INT 0
IRQ_INT 1
IRQ_INT 2
IRQ_INT 3
IRQ_INT 4
IRQ_INT 5
IRQ_INT 6
IRQ_INT 7
IRQ_INT 8
IRQ_INT 9
IRQ_INT 10
IRQ_INT 11
IRQ_INT 12
IRQ_INT 13
IRQ_INT 14
IRQ_INT 15


.globl IRQ_universal_handler_caller
IRQ_universal_handler_caller:
  pushad                    # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
  mov ax, ds                # Lower 16-bits of eax = ds.
  push ax                   # save the data segment descriptor
  # load the kernel data segment descriptor
#   mov ax, DATA_SEG
  mov ax, 0x10              # hardcoded for now
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  call IRQ_UniversalHandler
  # reload the original data segment descriptor
  pop ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  popad          # Pops edi,esi,ebp...
  add esp, 8     # Cleans up the pushed IRQ number
  sti
  iretd
.type IRQ_universal_handler_caller, @function
.size IRQ_universal_handler_caller, .-IRQ_universal_handler_caller
