#pragma once

#include <stdnoreturn.h>

noreturn void kernel_panic(const char* err_msg);
