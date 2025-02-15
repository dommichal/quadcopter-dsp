#include "nvm.h"

#include "main.h"

#include <string.h>

enum {
  WORD_SIZE = 4,
};

static volatile NVM_Storage persistentNvmStorage __attribute__((section(".nvm_section")));

static NVM_Storage storage; // RAM copy of the NVM storage

static uint32_t NVM_CalculateChecksum(NVM_Storage *config) {
  assert(config != NULL);

  uint32_t checksum = 0;
  uint32_t *data = (uint32_t *)config;

  // Calculate checksum for all fields except the checksum field
  for (size_t i = 0; i < sizeof(NVM_Storage) / WORD_SIZE - 1; ++i) {
    checksum += data[i];
  }

  return checksum;
}

static bool NVM_ChecksumIsValid(NVM_Storage *config) {
  assert(config != NULL);

  return NVM_CalculateChecksum(config) == config->checksum;
}

bool NVM_SaveData(NVM_Storage *config) {
  assert(config != NULL);

  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef flashErase;
  flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
  flashErase.PageAddress = NVM_ADDRESS;
  flashErase.Banks = FLASH_BANK_1;
  flashErase.NbPages = 1;

  uint32_t unused;
  if (HAL_OK != HAL_FLASHEx_Erase(&flashErase, &unused)) {
    return false;
  }

  config->checksum = NVM_CalculateChecksum(config);

  for (size_t i = 0; i < sizeof(NVM_Storage) / WORD_SIZE; ++i) {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, NVM_ADDRESS + WORD_SIZE * i, *((uint32_t *)config + i));
  }

  HAL_FLASH_Lock();
  return true;
}

void NVM_Init(void) {
  storage.imuCalibration.accOffset.x = persistentNvmStorage.imuCalibration.accOffset.x;
  storage.imuCalibration.accOffset.y = persistentNvmStorage.imuCalibration.accOffset.y;
  storage.imuCalibration.accOffset.z = persistentNvmStorage.imuCalibration.accOffset.z;
  storage.imuCalibration.gyroOffset.x = persistentNvmStorage.imuCalibration.gyroOffset.x;
  storage.imuCalibration.gyroOffset.y = persistentNvmStorage.imuCalibration.gyroOffset.y;
  storage.imuCalibration.gyroOffset.z = persistentNvmStorage.imuCalibration.gyroOffset.z;
  storage.checksum = persistentNvmStorage.checksum;

  if (!NVM_ChecksumIsValid(&storage)) {
    memset(&storage, 0, sizeof(NVM_Storage));
    NVM_SaveData(&storage);
  }
}

NVM_Storage *NVM_GetStorage(void) {
  if (!NVM_ChecksumIsValid(&storage)) {
    return NULL;
  }

  return &storage;
}