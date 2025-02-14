#if !defined(IBUS)
#define IBUS
#include "main.h"
#include "stdint.h"
#include "stdbool.h"

#define IBUS_ID                     0x40
#define IBUS_DATAFRAME_SIZE         32
#define IBUS_NUMBER_0F_CHANELS      14
#define IBUS_CHANELS_BYTES          28

typedef struct ibus_t
{
    UART_HandleTypeDef *huart;
    uint8_t dma_buffer[IBUS_DATAFRAME_SIZE];
}ibus_t;


void IBUS_Init(ibus_t *ibus);   
void IBUS_Start(ibus_t *ibus);
void IBUS_Stop();
HAL_StatusTypeDef IBUS_Receive_DMA_Complete(ibus_t *ibus, uint16_t ch_data[IBUS_NUMBER_0F_CHANELS]);

#endif // SBUS
