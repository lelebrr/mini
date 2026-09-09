#pragma once
#include <stdint.h>
void hal_touch_init(void);
int hal_touch_read(int *x, int *y);
const char* hal_touch_get_chip_name(void);
