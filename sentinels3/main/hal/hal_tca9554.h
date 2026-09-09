#pragma once
#include <stdint.h>
#include "sentinel_hw.h"
void hal_tca9554_init(void);
void hal_tca9554_write_pin(int pin, int val);
int hal_tca9554_read_pin(int pin);
