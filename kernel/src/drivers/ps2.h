#pragma once

#include <stdint.h>

#define PS2_COMMAND_TEST          0xAA
#define PS2_COMMAND_TEST_PORT1    0xAB
#define PS2_COMMAND_TEST_PORT2    0xA9
#define PS2_COMMAND_READ_CFG      0x20
#define PS2_COMMAND_WRITE_CFG     0x60
#define PS2_COMMAND_DISABLE_PORT1 0xAD
#define PS2_COMMAND_DISABLE_PORT2 0xA7
#define PS2_COMMAND_ENABLE_PORT1  0xAE
#define PS2_COMMAND_ENABLE_PORT2  0xA8
#define PS2_COMMAND_WRITE_PORT2   0xD4

void    PS2_init(void);
void    PS2_send_command(uint8_t command);
uint8_t PS2_read_status(void);
