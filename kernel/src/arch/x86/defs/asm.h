/********************
 * ASM definitions  *
 ********************/
#pragma once

#define ASM_BEGIN(x) \
    .globl x; \
    x:

#define ASM_END(x) \
    .type x, @function; \
    .size x, .-x;
