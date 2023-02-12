#pragma once

#include <stdnoreturn.h>

#define KERNEL_PANIC(err_msg) panic(err_msg, __FILE__, __LINE__)

// TODO use sprintf instead in the macro and here 1 param only....
// also define an error/panic/critical without clearing the screen
noreturn void panic(const char* err_msg, const char* filename, int line);
