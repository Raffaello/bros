#pragma once

#include <stdnoreturn.h>

#define KERNEL_PANIC(err_msg) panic(err_msg, __FILE__, __LINE__)

noreturn void panic(const char* err_msg, const char* filename, int line);
