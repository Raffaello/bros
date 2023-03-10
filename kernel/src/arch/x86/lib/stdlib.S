/* -*-Asm-*- */
/* x86 specific code */

// 2560 throws divide error: 2560 % 10 = 256 (greater than 255)
// requires to use bigger registers and handling the INT0 exception

# .code32
# .intel_syntax noprefix
# .text

# #include <arch/x86/defs/asm.h>

# ASM_BEGIN(itoa10)
#   # return value in eax
#   push ebp
#   push ebx
#   mov ebp, esp
#   # 12, because 4 for ebp, 4 ebx and other 4 for EIP due to the call/ret
#   mov eax, [ebp + 12]   # value
#   xor ecx,ecx           # string size
#   mov ebx, 10           # base
#   xor edx,edx
# itoa10_div_loop:
#   div bl                # AL := Quotient, AH := Remainder
#   push ax               # the remainder is the digit
#   inc ecx
#   and ax, 0xFF
#   jnz itoa10_div_loop

#   mov ebx, [ebp + 12+4]   # str ptr
#   mov edi, ebx

# itoa10_store_loop:
#   # 1 digits is always guaranteed: 0
#   pop ax
#   mov al, ah
#   add al, '0'
#   stosb
#   loop itoa10_store_loop
#   xor al,al
#   stosb     # null terminated string
#   mov eax, ebx

#   pop ebx
#   pop ebp
#   ret
# ASM_END(itoa10)

# ASM_BEGIN(itoa16)
# itoa16:
#   # return value in eax
#   push ebp
#   push ebx
#   mov ebp, esp
#   # 12, because 4 for ebp, 4 ebx and other 4 for EIP due to the call/ret
#   mov eax, [ebp + 12]   # value
#   xor ecx,ecx           # string size
#   mov ebx, 16           # base
#   xor edx,edx
# itoa16_div_loop:
#   div bl                # AL := Quotient, AH := Remainder
#   push ax               # the remainder is the digit
#   inc ecx
#   and ax, 0xFF
#   jnz itoa16_div_loop

#   mov ebx, [ebp + 12+4]   # str ptr
#   mov edi, ebx

# itoa16_store_loop:
#   # 1 digits is always guaranteed: 0
#   pop ax
#   mov al, ah
#   cmp al, 9
#   jg itoa16_store_loop_alpha
#   add al, '0'
#   jmp itao16_store_loop_stosb
# itoa16_store_loop_alpha:
#   add al, 'A'-10
# itao16_store_loop_stosb:
#   stosb
#   loop itoa16_store_loop
#   xor al,al
#   stosb     # null terminated string
#   mov eax, ebx
#   pop ebx
#   pop ebp
#   ret
# ASM_END(itoa16)
