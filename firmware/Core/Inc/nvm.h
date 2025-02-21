#ifndef NVM_H
#define NVM_H

#include "hal_imu.h"
#include "hal_radio.h"

#include <stdbool.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NVM_ADDRESS)
#error "NVM_ADDRESS not defined"
#endif

typedef struct __attribute__((packed)) {
    HAL_IMU_Calibration imuCalibration;
    uint32_t checksum;
} NVM_Storage __attribute__((aligned(4)));

void NVM_Init(void);

bool NVM_SaveData(NVM_Storage *config);

NVM_Storage *NVM_GetStorage(void);

#ifdef __cplusplus
}
#endif

#endif // NVM_H