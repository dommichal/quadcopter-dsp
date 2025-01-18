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
  MPU6050_STRUCT mpu;
  HAL_IMU_conversion_complete_callback_t imu_readout_callback;
  float acc[3];
  float gyro[3];
} IMU_DEVICE;

static IMU_DEVICE imu;

void HAL_IMU_init(HAL_IMU_conversion_complete_callback_t imu_readout_callback) {
  imu.mpu.hi2c = &hi2c1;
  imu.mpu.mpu_acc_buff = imu.acc;
  imu.mpu.mpu_gyro_buff = imu.gyro;
  MPU6050_config mpu_config = MPU_get_default_cfg();
  MPU_init(&imu.mpu, &mpu_config);

  imu.imu_readout_callback = imu_readout_callback;
}

void HAL_IMU_deinit() {}

void HAL_IMU_proc() {}

void HAL_IMU_start_conversion() { MPU_clear_int(&imu.mpu); }

void HAL_IMU_calibrate() {
  MPU_measure_acc_offset(&imu.mpu, IMU_ACC_CALIBRATION_SAMPLES);
}

void HAL_IMU_request_readout() { MPU_read_acc_gyro_DMA(&imu.mpu); }

void HAL_IMU_readout() {
  MPU_read_acc_gyro_DMA_complete(&imu.mpu);
  imu.imu_readout_callback(imu.acc, imu.gyro);
}