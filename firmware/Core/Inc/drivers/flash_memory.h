#if !defined(FLASH_MEMORY)
#define FLASH_MEMORY

#include <stdint.h>
#include <main.h>

#define LAST_PAGE_ADDRESS       0x08007C00U
#define END_OF_FLASH_ADDRESS    0x080FC800U

void FlashMemory_SaveData(uint32_t memory_address, uint32_t *data, uint16_t data_length);
void FlashMemory_ReadData(uint32_t memory_address, uint32_t *data, uint16_t data_length);

#endif // FLASH
