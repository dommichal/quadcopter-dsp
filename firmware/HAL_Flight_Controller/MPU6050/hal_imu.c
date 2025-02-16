/**
 * @file imu.c
 * @author Dominik Michalczyk
 *
 * IMU module platform specific implementation.
 */

#include "hal_imu.h"

#include "mpu6050.h"

#include <assert.h>

enum {
  IMU_GYRO_CALIBRATION_SAMPLES = 10000,
  IMU_ACC_CALIBRATION_SAMPLES = 15000
};

typedef struct {
  HAL_IMU_OnConversionComplete imu_readout_callback;
  float acc[3];
  float gyro[3];
  MPU6050_Device mpu;
} IMU_Device;

static IMU_Device imu;

static_assert(sizeof(HAL_IMU_Offset) == sizeof(MPU6050_Offset), "Offset size mismatch");
static_assert(sizeof(HAL_IMU_Calibration) == sizeof(MPU6050_Calibration), "Calibration size mismatch");

void HAL_IMU_init(HAL_IMU_OnConversionComplete imu_readout_callback, HAL_IMU_Calibration *calibration) {
  assert(NULL != imu_readout_callback);
  assert(NULL != calibration);

  imu.mpu.hi2c = &hi2c1;
  MPU6050_Config mpu_config = MPU_GetDefaultConfiguration();

  MPU_Init(&imu.mpu, imu.acc, imu.gyro, &mpu_config, (MPU6050_Calibration*)calibration);

  imu.imu_readout_callback = imu_readout_callback;
}

void HAL_IMU_deinit() {} // Stub

void HAL_IMU_proc() {} // Stub

void HAL_IMU_start_conversion() { MPU_ClearInterrupt(&imu.mpu); }

void HAL_IMU_calibrate(HAL_IMU_Calibration *calibration) {
  MPU6050_Offset *gyro_offset = (MPU6050_Offset*)&calibration->gyroOffset;
  MPU6050_Offset *acc_offset = (MPU6050_Offset*)&calibration->accOffset; 
  MPU_MeasureAccelerometerOffset(&imu.mpu, acc_offset, IMU_ACC_CALIBRATION_SAMPLES);
  MPU_MeasureGyroOffset(&imu.mpu, gyro_offset, IMU_GYRO_CALIBRATION_SAMPLES);
}

void HAL_IMU_request_readout() { MPU_ReadAccGyroDMA(&imu.mpu); }

void HAL_IMU_readout() {
  MPU_ReadAccGyroDMAComplete(&imu.mpu);
  imu.imu_readout_callback(imu.acc, imu.gyro);
}