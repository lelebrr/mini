#pragma once
#include <stdint.h>
#include <stdbool.h>
void hal_rtc_init(void);
bool hal_rtc_get_time(char *buf, size_t len);
void hal_rtc_set_time(int y, int m, int d, int h, int min, int s);
