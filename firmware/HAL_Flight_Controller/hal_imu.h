/**
 * @file imu.h
 * @author Dominik Michalczyk
 *
 * IMU module platform independent interface.
 */

#ifndef IMU_H_
#define IMU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define HAL_IMU_INTERFACE_I2C

typedef struct {
  float x;
  float y;
  float z;
} HAL_IMU_Offset;

typedef struct __attribute__((packed)) {
  HAL_IMU_Offset gyroOffset;
  HAL_IMU_Offset accOffset;
} HAL_IMU_Calibration;

typedef void (*HAL_IMU_OnConversionComplete)(const float *acc, const float *gyro);

void HAL_IMU_init(HAL_IMU_OnConversionComplete imu_readout_callback, HAL_IMU_Calibration *calibration);
void HAL_IMU_deinit(void);
void HAL_IMU_calibrate(HAL_IMU_Calibration *calibration);
void HAL_IMU_proc(void);
void HAL_IMU_start_conversion(void);
void HAL_IMU_stop_conversion();
void HAL_IMU_request_readout(void);
void HAL_IMU_readout(void);

#ifdef __cplusplus
}
#endif

#endif // IMU_H_
