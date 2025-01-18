#if !defined(LOGGER)
#define LOGGER

#include "w25q64.h"
#include "spi.h"

#include <stdbool.h>
#include <stdint.h>

#define MAX_BYTES_IN_PAGE   255
#define INITIAL_PAGE_POINTER 0x001000

void Logger_Init();
void Logger_Log_Data(uint8_t *data, uint8_t size);
void Logger_Read_Data(uint8_t *data, uint8_t size);
void Logger_Erase();

#endif // LOGGER


