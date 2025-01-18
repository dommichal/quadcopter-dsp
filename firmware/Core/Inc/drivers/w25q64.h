#if !defined(W25Q64)
#define W25Q64

#include "spi.h"

#define W25Q64_STATUS1			0x05
#define W25Q64_STATUS2			0x35
#define W25Q64_JEDEC_ID         0x9F
#define W25Q64_CHIP_ERASE       0xC7
#define W25Q64_WRITE_DISABLE    0x04
#define W25Q64_WRITE_ENABLE     0x06
#define W25Q64_PAGE_PROGRAM     0x02
#define W25Q64_READ_DATA		0x03   
#define W25Q64_FAST_READ		0x0B   

#define W25Q64_ID               0x17
#define W25Q64_DUMMY_BYTE       0xA5
#define W25Q64_BUSY_BIT			0x01

#define W25Q64_BLOCK_CNT        128
#define W25Q64_SECTOR_CNT       W25Q64_BLOCK_CNT * 16 
#define W25Q64_PAGE_CNT         W25Q64_SECTOR_CNT * 16
#define W25Q64_SECTOR_SIZE      4096
#define w25Q64_PAGE_SIZE        256
#define W25Q64_ADDRESS_SIZE		3

typedef struct W25Q64_STRUCT
{
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *cs_gpio;
	uint16_t cs_pin;
}W25Q64_STRUCT;


HAL_StatusTypeDef W25Q64_Init(W25Q64_STRUCT *w25q);
HAL_StatusTypeDef W25Q64_Chip_Erase(W25Q64_STRUCT *w25q);
HAL_StatusTypeDef W25Q64_Write_Page(W25Q64_STRUCT *w25q, uint32_t address, uint8_t *data_buf, uint16_t size);
HAL_StatusTypeDef W25Q64_Read_Data(W25Q64_STRUCT *w25q, uint32_t address, uint8_t *data_buf, uint16_t size);
void W25Q64_Test_Function(W25Q64_STRUCT *w25q);

// void W25Q64_Test_Function(W25Q64_STRUCT *w25q);

#endif // W25Q64
