#pragma once
void hal_imu_init(void);
void hal_imu_read(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
