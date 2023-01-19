/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text

.globl itoa10
itoa10:
  # return value in eax
  push ebp
  push ebx
  mov ebp, esp
  # 8, because 4 for ebp and other for 4 EIP due to the call/ret
  mov eax, [ebp + 8]    # value
  xor ecx,ecx           # string size
  mov ebx, 10           # base
itoa10_div_loop:
  xor edx,edx
  div bl                # AL := Quotient, AH := Remainder
  push ax               # the remainder is the digit
  inc ecx
  or ax, ax
  jnz itoa10_div_loop
  mov esi, [ebp + 12]   # str ptr

itoa10_store_loop:
  # 1 digits is always guaranteed: 0
  pop ax
  add al, '0'
  stosb
  loop itoa10_store_loop
  xor al,al
  stosb     # null terminated string
  mov eax, ebx

  pop ebx
  pop ebp
  ret
.type itoa10, @function
.size itoa10, .-itoa10