/**
 * @author Dominik Michalczyk
 * @date  25-12-2023
 */
#include <w25q64.h>


HAL_StatusTypeDef W25Q64_Read_Bytes(W25Q64_STRUCT *w25q, uint8_t address, uint8_t *data_buf, uint16_t size){
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);

    HAL_StatusTypeDef status = HAL_SPI_Transmit(w25q->hspi, &address, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(w25q->hspi) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }

    uint8_t tx_dummy = W25Q64_DUMMY_BYTE; //TODO: check if this sending dummy byte for each data buffer byte
    status = HAL_SPI_TransmitReceive(w25q->hspi, &tx_dummy, data_buf, size, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(w25q->hspi) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }

    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);

    return status;
}

HAL_StatusTypeDef W25Q64_Write_Address(W25Q64_STRUCT *w25q, uint32_t address){    
    uint8_t tx_buff[3] = {(address & 0xFF0000) >> 16, (address & 0xFF00) >> 8, address & 0xFF};  
    HAL_StatusTypeDef status = HAL_SPI_Transmit(w25q->hspi, tx_buff, W25Q64_ADDRESS_SIZE, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(w25q->hspi) != HAL_SPI_STATE_READY);    
    return status;
}

HAL_StatusTypeDef W25Q64_Write_Command(W25Q64_STRUCT *w25q, uint8_t command){
    uint8_t tx_data = command;
    HAL_StatusTypeDef status = HAL_SPI_Transmit(w25q->hspi, &tx_data, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(w25q->hspi) != HAL_SPI_STATE_READY);

    return status;
}

HAL_StatusTypeDef W25Q64_Wait_For_Unoccupied_Device(W25Q64_STRUCT *w25q){
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    uint8_t data = 0;
    HAL_StatusTypeDef status;
    do
    {
        status = W25Q64_Read_Bytes(w25q, W25Q64_STATUS1, &data, 1);
        while(HAL_SPI_GetState(w25q->hspi) != HAL_SPI_STATE_READY);
    } while (data & W25Q64_BUSY_BIT);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);

    return status;
}

HAL_StatusTypeDef W25Q64_Init(W25Q64_STRUCT *w25q){
    uint8_t data[3] = {0};

    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    HAL_Delay(100);

    HAL_StatusTypeDef status = W25Q64_Read_Bytes(w25q, W25Q64_JEDEC_ID, data, 3);
    
    if(data[2] != W25Q64_ID){
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    status = W25Q64_Write_Command(w25q, W25Q64_WRITE_DISABLE);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);

    return status;
}

HAL_StatusTypeDef W25Q64_Chip_Erase(W25Q64_STRUCT *w25q){
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = W25Q64_Write_Command(w25q, W25Q64_WRITE_ENABLE);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    if(status != HAL_OK){ return status; }
    
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    status = W25Q64_Write_Command(w25q, W25Q64_CHIP_ERASE);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    if(status != HAL_OK){ return status; }

    status =  W25Q64_Wait_For_Unoccupied_Device(w25q); 
    if(status != HAL_OK){ return status; }
    
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    status = W25Q64_Write_Command(w25q, W25Q64_WRITE_DISABLE);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    return status;
}

HAL_StatusTypeDef W25Q64_Write_Page(W25Q64_STRUCT *w25q, uint32_t address, uint8_t *data_buf, uint16_t size){
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = W25Q64_Write_Command(w25q, W25Q64_WRITE_ENABLE);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    if(status != HAL_OK){ return status; }

    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    W25Q64_Write_Command(w25q, W25Q64_PAGE_PROGRAM);
    W25Q64_Write_Address(w25q, address); 
    status = HAL_SPI_Transmit(w25q->hspi, data_buf, size, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(w25q->hspi) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    if(status != HAL_OK){ return status; }
    
    W25Q64_Wait_For_Unoccupied_Device(w25q);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    status = W25Q64_Write_Command(w25q, W25Q64_WRITE_DISABLE);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    return status;
}

HAL_StatusTypeDef W25Q64_Read_Data(W25Q64_STRUCT *w25q, uint32_t address, uint8_t *data_buf, uint16_t size){
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_RESET);
    W25Q64_Write_Command(w25q, W25Q64_READ_DATA);
    HAL_StatusTypeDef status = W25Q64_Write_Address(w25q, address);     
    
    status = HAL_SPI_Receive(w25q->hspi, data_buf, size, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(w25q->hspi) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(w25q->cs_gpio, w25q->cs_pin, GPIO_PIN_SET);
    return status;
}

HAL_StatusTypeDef W25Q64_Write_Page_DMA(W25Q64_STRUCT *w25q){
    return 0;
}

void W25Q64_Test_Function(W25Q64_STRUCT *w25q){
    uint32_t address = 0x201040; 
    uint8_t test_data[20] = {0x00};
    test_data[5] = 0x11;

    W25Q64_Write_Page(w25q, address, test_data, 20);


    address = 0x201039;
    uint8_t data_buffer[40] = {0x00};
    W25Q64_Read_Data(w25q, address, data_buffer, 40);
    test_data[0] = 1;
}