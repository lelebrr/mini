#pragma once
#include <stdbool.h>
#include <stdint.h>
void hal_pmu_init(void);
float hal_pmu_get_battery_voltage(void);
int hal_pmu_get_battery_percent(void);
bool hal_pmu_is_charging(void);
float hal_pmu_get_temperature(void);
