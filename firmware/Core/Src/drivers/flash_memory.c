/**
 * @file flash_memory.c
 * @author Dominik Michalczyk
 * @brief Flash memory liblary for saving user data to flash
 * @version 0.1
 * @date 2024-01-26
 */
#include <drivers/flash_memory.h>
#include <stm32f1xx_it.h>

void FlashMemory_SaveData(uint32_t memory_address, uint32_t *data, uint16_t data_length){
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flash_erase;
    flash_erase.TypeErase = FLASH_TYPEERASE_PAGES;
    flash_erase.PageAddress = LAST_PAGE_ADDRESS;
    flash_erase.Banks = FLASH_BANK_1;
    flash_erase.NbPages = 1;

    uint32_t unused;
    if (HAL_OK != HAL_FLASHEx_Erase(&flash_erase, &unused)) {
        return;
    }

    for (size_t i = 0; i < data_length; ++i) {        
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, memory_address, *data);
    }

    HAL_FLASH_Lock();
}

void FlashMemory_ReadData(uint32_t memory_address, uint32_t *data, uint16_t data_length) {
    for (size_t i = 0; i < data_length; i++) {
        *(data + i) = *((uint32_t *)memory_address + i);
    }
}

// void Flash_Read_Data(uint32_t memory_address, uint8_t *data, uint16_t data_length){
//     for (size_t i = 0; i < data_length; i++)
//     {
//         *(data + i) = *((uint8_t *)memory_address + i);
//     }
    
// }
