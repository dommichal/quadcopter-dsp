/**
 * @file flash_memory.c
 * @author Dominik Michalczyk
 * @brief Flash memory liblary for saving user data to flash
 * @version 0.1
 * @date 2024-01-26
 */
#include <drivers/flash_memory.h>
#include <stm32f1xx_it.h>

/**
 * @brief this function stores data on the last page of the flash memory
 * @param memory_address must be a number between LAST_PAGE_ADDRESS and 
 *        END_OF_FLASH_ADDRESS
 * @param data specifies the data to be programmed
 * @param data_length length of the data in bytes
 */
HAL_StatusTypeDef Flash_Save_Data(uint32_t memory_address, uint8_t *data, uint16_t data_length){
    FLASH_EraseInitTypeDef flash_erase = {0};
    uint16_t i = 0;
    uint8_t word_data[WORD_SIZE];

    if(memory_address < LAST_PAGE_ADDRESS){
        return HAL_ERROR;
    }

    HAL_FLASH_Unlock();

    flash_erase.TypeErase = FLASH_TYPEERASE_PAGES;
    flash_erase.PageAddress = LAST_PAGE_ADDRESS;
    flash_erase.NbPages = 1;

    uint32_t status;
    HAL_FLASHEx_Erase(&flash_erase, &status);

    // write number of words to flash 
    while (i <= data_length)
    {
        word_data[i % WORD_SIZE] = data[i];
        i++;
        if(i % WORD_SIZE == 0){
            HAL_FLASH_Program(TYPEPROGRAM_WORD, memory_address + i
                              - WORD_SIZE, *((uint32_t *)word_data));
        }
    }
    // if data does not fulfill the word, save the incomplete word
    if(i % WORD_SIZE != 0){
        HAL_FLASH_Program(TYPEPROGRAM_WORD, memory_address + i -
                          i % WORD_SIZE, *((uint32_t *)word_data));
    }
    
    HAL_FLASH_Lock();
    return HAL_OK;
}

void Flash_Read_Data(uint32_t memory_address, uint8_t *data, uint16_t data_length){
    for (size_t i = 0; i < data_length; i++)
    {
        *(data + i) = *((uint8_t *)memory_address + i);
    }
    
}
