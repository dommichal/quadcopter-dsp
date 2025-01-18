#if !defined(FLASH_MEMORY)
#define FLASH_MEMORY
#include <main.h>

#define LAST_PAGE_ADDRESS       0x08007C00U
#define END_OF_FLASH_ADDRESS    0x08007FFFU
#define WORD_SIZE               4U

HAL_StatusTypeDef Flash_Save_Data(uint32_t memory_address, uint8_t *data, uint16_t data_length);
void Flash_Read_Data(uint32_t memory_address, uint8_t *data, uint16_t data_length);

#endif // FLASH
