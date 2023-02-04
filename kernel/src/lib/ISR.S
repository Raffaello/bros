/* -*-Asm-*- */
/* x86 specific code */

# Interrupt Service Routines
# Interrupts are defined in
# #include <defs/interrupts.h>

.code32
.intel_syntax noprefix
.text

.macro ISR_INT x, e
.globl ISR_INT_\x
ISR_INT_\x:
  cli
  .if \e == 0
    push \e
  .endif
  push \x
  jmp ISR_universal_handler_caller
.type ISR_INT_\x, @function
.size ISR_INT_\x, .-ISR_INT_\x
.endm

# ISR INT_Divide_Error
# Can't make the macro working calling the other macro
ISR_INT 0, 0
ISR_INT 1, 0
ISR_INT 2, 0
ISR_INT 3, 0
ISR_INT 4, 0
ISR_INT 5, 0
ISR_INT 6, 0
ISR_INT 7, 0
ISR_INT 8, 1
ISR_INT 9, 0
ISR_INT 10, 1
ISR_INT 11, 1
ISR_INT 12, 1
ISR_INT 13, 1
ISR_INT 14, 1
ISR_INT 15, 0
ISR_INT 16, 0
ISR_INT 17, 0
ISR_INT 18, 0
ISR_INT 19, 0
ISR_INT 20, 0
ISR_INT 21, 0

# .macro ISR_INT_init from, to
#    ISR_INT \from
#   .long \from
#   .if \to-\from
#     ISR_INT_init (\from+1),\to
#   .endif
# .endm

# ISR_INT_init 0, 255

.globl ISR_universal_handler_caller
ISR_universal_handler_caller:
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

  call ISR_UniversalHandler
  # reload the original data segment descriptor
  pop ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  popad          # Pops edi,esi,ebp...
  add esp, 8     # Cleans up the pushed error code and pushed ISR number
  sti
  iretd
.type ISR_universal_handler_caller, @function
.size ISR_universal_handler_caller, .-ISR_universal_handler_caller
