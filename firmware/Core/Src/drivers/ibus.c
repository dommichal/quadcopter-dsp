#include <drivers/ibus.h>
volatile bool ibus_flag;

void IBUS_Init(ibus_t *ibus){

}

void IBUS_Decodemessage(uint8_t buff[IBUS_DATAFRAME_SIZE], uint16_t ch_data[IBUS_NUMBER_0F_CHANELS]){
    for (size_t ch_idx = 0, buff_idx = 2; buff_idx < IBUS_CHANELS_BYTES + 2; ch_idx++, buff_idx += 2)
    {
        ch_data[ch_idx] = buff[buff_idx + 1] << 8 | buff[buff_idx];
    }
}

HAL_StatusTypeDef IBUS_Receive_DMA(ibus_t *ibus){
    return HAL_UART_Receive_DMA(ibus->huart, ibus->dma_buffer, IBUS_DATAFRAME_SIZE);
}

HAL_StatusTypeDef IBUS_Receive_DMA_Complete(ibus_t *ibus, uint16_t ch_data[IBUS_NUMBER_0F_CHANELS]){
    if(ibus->dma_buffer[0] == IBUS_DATAFRAME_SIZE && ibus->dma_buffer[1] == IBUS_ID){
        IBUS_Decodemessage(ibus->dma_buffer, ch_data);
    }
    
    if(ibus_flag == true){
        return IBUS_Receive_DMA(ibus);
    }

    return HAL_ERROR;
}

void IBUS_Start(ibus_t *ibus){
    ibus_flag = true;
    IBUS_Receive_DMA(ibus);
}

void IBUS_Stop(){
    ibus_flag = false;
}