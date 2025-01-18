/**
 * @file nrf24l01.c
 * @author Dominik Michalczyk
 * @brief nRF24l01+ liblary 
 * @version 0.1
 * @date 2024-01-26
 */

#include "nrf24l01.h"

NRF24L01_CONFIG nrf24l01_default_config = {
    .rx_int = RX_DR_INT_SET,
    .tx_int = TX_DS_INT_RESET,
    .max_rt_int = MAX_RT_INT_RESET,
    .en_crc = EN_CRC_SET,
    .crco_enc = ONE_BYTE_ENCODING,
    .rx_tx_control = PRX,
    .rx_tx_addr_width = FIVE_BYTES_ADDR,
    .lna_hcurr = LNA_HCURR_SET,
    .data_rate = ONE_MBPS_DATA_RATE,
    .power_sel = RF_POWER_2,
    .re_transmission_delay = 0x4,
    .re_transmission_num = 0xF,
    .chanel = 88
};

/**
 * @brief Initialize device 
 * Call this function at the beginning
 */
HAL_StatusTypeDef NRF24L01_Init(NRF24L01_STRUCT *nrf24l01, NRF24L01_CONFIG *nrf24l01_cfg)
{
    uint8_t data; HAL_StatusTypeDef status; 

    /* Clear Payload Flag */
    nrf24l01->payloadFlag = 0;

    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->cePin, GPIO_PIN_RESET);

    HAL_Delay(POWER_ON_TIME);

    /* Initialize Tx Buffer */
    nrf24l01->txBuff[0] = R_RX_PAYLOAD;
    for (size_t i = 1; i < MAX_RX_PAYLOAD_SIZE; i++)
    {
        nrf24l01->txBuff[i] = 0xFF;
    }

    /* Power Down Device & Disable Interrupts */
    data = (1 << MASK_RX_DR | 1 << MASK_TX_DS | 1 << MASK_MAX_RT);
    status = NRF24L01_Write_Byte(nrf24l01, NRF_CONFIG, data);
    if(status != HAL_OK){ return status; }

    /* Setup Address Widths */
    data = nrf24l01_cfg->rx_tx_addr_width;
    status = NRF24L01_Write_Byte(nrf24l01, SETUP_AW, data);
    if(status != HAL_OK){ return status; }
    
    /* Setup Retransmission */
    data = (nrf24l01_cfg->re_transmission_delay << ARD | nrf24l01_cfg->re_transmission_num);
    status = NRF24L01_Write_Byte(nrf24l01, SETUP_RETR, data);
    if(status != HAL_OK){ return status; }

    /* RF Setup */
    data = (nrf24l01_cfg->data_rate << RF_DR| nrf24l01_cfg->power_sel << RF_PWR |
            nrf24l01_cfg->lna_hcurr);
    status = NRF24L01_Write_Byte(nrf24l01, RF_SETUP, data);
    if(status != HAL_OK){ return status; }

    /* Disable Dynamic Payloads */
    data = 0;
    status = NRF24L01_Write_Byte(nrf24l01, DYNPD, data);
    if(status != HAL_OK){ return status; }

    /* FLush RX TX */
    NRF24L01_Flush_Tx(nrf24l01);
    NRF24L01_Flush_Tx(nrf24l01);
    
    /* Config & Start Device*/
    data = (nrf24l01_cfg->rx_int << MASK_RX_DR | nrf24l01_cfg->tx_int << MASK_TX_DS |
            nrf24l01_cfg->max_rt_int << MASK_MAX_RT | nrf24l01_cfg->en_crc << EN_CRC_MASK |
            nrf24l01_cfg->crco_enc << CRCO | 1 << PWR_UP | nrf24l01_cfg->rx_tx_control); 

    status = NRF24L01_Write_Byte(nrf24l01, NRF_CONFIG, data);
    if(status != HAL_OK){ return status; }

    HAL_Delay(2);

    NRF24L01_Chanel(nrf24l01, nrf24l01_cfg->chanel);

    return HAL_OK;
}

/**
 * @brief Set rf chanel 
 * f = 2400 + rfCh [MHz]
 * max rfCh = 127
 */
HAL_StatusTypeDef NRF24L01_Chanel(NRF24L01_STRUCT *nrf24l01, uint8_t rfCh)
{
    if(rfCh > MAX_RF_CH){
        rfCh = MAX_RF_CH;
    }

    return NRF24L01_Write_Byte(nrf24l01, RF_CH, rfCh);
}

/**
 * @brief Read one byte from device 
 */
HAL_StatusTypeDef NRF24L01_Read_Byte(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint8_t *data)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);
    uint8_t tx_buff[2] = {R_REGISTER | addr, RF24_NOP};
    uint8_t rx_buff[2];
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(nrf24l01->spiHandle, tx_buff, rx_buff, 2, HAL_MAX_DELAY);
    if(status != HAL_OK){ return status; }
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    *data = rx_buff[1];
    
    return status;
}

/**
 * @brief Write one byte to device 
 */
HAL_StatusTypeDef NRF24L01_Write_Byte(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint8_t data)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);
    uint8_t tx_buff[2] = {W_REGISTER | addr, data};
    uint8_t rx_buff[2] = {0};
    //HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, tx_buff, 2, HAL_MAX_DELAY);
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(nrf24l01->spiHandle, tx_buff, rx_buff, 2, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);

    return status;
}

/**
 * @brief Write number of bytes to device
 */
HAL_StatusTypeDef NRF24L01_Write(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint64_t data, uint8_t len)
{
    uint8_t *data_ptr = (uint8_t*) &data;
    HAL_GPIO_WritePin( nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);
    
    uint8_t tx_buff = {W_REGISTER | addr};
    HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, &tx_buff, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }

    status = HAL_SPI_Transmit(nrf24l01->spiHandle, data_ptr, len, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }

    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    
    return status;
}

/**
 * @brief Read number of bytes from device
 */
HAL_StatusTypeDef NRF24L01_Read(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint64_t *data, uint8_t len)
{
    uint8_t *data_ptr = (uint8_t*) data;
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);

    uint8_t tx_buff = (R_REGISTER | addr);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, &tx_buff, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }

    uint8_t tx_dummy;
    status = HAL_SPI_TransmitReceive(nrf24l01->spiHandle, &tx_dummy, data_ptr, len, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }

    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    
    return status;
}


/**
 * @brief Flush Tx
 */
HAL_StatusTypeDef NRF24L01_Flush_Tx(NRF24L01_STRUCT *nrf24l01)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);
    uint8_t tx_buff = FLUSH_TX; 
    HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, &tx_buff, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    
    return status ;
}

/**
 * @brief Flush Rx
 */
HAL_StatusTypeDef NRF24L01_Flush_Rx(NRF24L01_STRUCT *nrf24l01)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);
    uint8_t tx_buff = FLUSH_RX; 
    HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, &tx_buff, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    
    return status;
}

/** 
 * @brief Write payload to tx fifo 
 */
HAL_StatusTypeDef NRF24L01_Write_Tx_Payload(NRF24L01_STRUCT *nrf24l01, void *payload, uint8_t len)
{
    uint8_t *data = (uint8_t*) payload; 
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);

    uint8_t tx_buff = W_TX_PAYLOAD;
    HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, &tx_buff, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }
    
    status = HAL_SPI_Transmit(nrf24l01->spiHandle, data, len, HAL_MAX_DELAY); 
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);      

    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    
    return status;
}

/**
 *  @brief Send payload over the air 
 */
HAL_StatusTypeDef NRF24L01_Send_Payload(NRF24L01_STRUCT *nrf24l01)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->cePin, GPIO_PIN_SET);
    TIM1_Delay_Microseconds(15);
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->cePin, GPIO_PIN_RESET);

    uint8_t data;
    HAL_StatusTypeDef status = NRF24L01_Read_Byte(nrf24l01, NRF_STATUS, &data);
    if(status != HAL_OK){ return status; }                                    
    /* If max retransmissions occur clear MAX_RT flag to enable further communication */
    return NRF24L01_Write_Byte(nrf24l01, NRF_STATUS, (data | (1 << MASK_MAX_RT)));
}

/** 
 * @brief Send data to tx FIFO
 */
HAL_StatusTypeDef NRF24L01_Send(NRF24L01_STRUCT *nrf24l01, void *data, uint8_t len){
    HAL_StatusTypeDef status = NRF24L01_Flush_Tx(nrf24l01);

    status = NRF24L01_Write_Tx_Payload(nrf24l01, data, len);
    if(status != HAL_OK){ return status; }

    return NRF24L01_Send_Payload(nrf24l01);
}

/** 
 * @brief Start listening for packages 
 */
HAL_StatusTypeDef NRF24L01_Start_Listening(NRF24L01_STRUCT *nrf24l01){
    //flush rx before reseting interrupts
    NRF24L01_Flush_Rx(nrf24l01);
    volatile HAL_StatusTypeDef status =  NRF24L01_Write_Byte(nrf24l01, NRF_STATUS, (1<<MASK_RX_DR) |(1<<MASK_TX_DS) | (1<<MASK_MAX_RT));
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->cePin, GPIO_PIN_SET);    
    TIM1_Delay_Microseconds(150);
    return status;
}
/**
 * @brief Stop listening for packages 
 */
void NRF24L01_Stop_Listening(NRF24L01_STRUCT *nrf24l01){
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->cePin, GPIO_PIN_RESET);    
    TIM1_Delay_Microseconds(150);
}


/** 
 * @brief Check if packet is in rx fifo 
 */
HAL_StatusTypeDef NRF24L01_Packet_Available(NRF24L01_STRUCT *nrf24l01){
    uint8_t data;
    NRF24L01_Read_Byte(nrf24l01, FIFO_STATUS, &data);

    if(!(data & 1<<RX_EMPTY)){ 
        return HAL_OK; 
    }
    
    return HAL_TIMEOUT;
}

/** 
 * @brief Read payload from rx fif0 
 */
/*  */
HAL_StatusTypeDef NRF24L01_Read_Payload(NRF24L01_STRUCT *nrf24l01, uint8_t *data, uint8_t len)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);

    uint8_t tx_buff = R_RX_PAYLOAD;
    HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, &tx_buff, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }
    
    status = HAL_SPI_Receive(nrf24l01->spiHandle, data, len, HAL_MAX_DELAY); 
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);      
    if(status != HAL_OK){ return status; }                                    

    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);

    return status;
}

/**
 * @brief Read payload using dma
 */
HAL_StatusTypeDef NRF24L01_Read_PayloadDMA(NRF24L01_STRUCT *nrf24l01, uint8_t len)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);
    
    nrf24l01->payloadFlag = 1;                               

    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(nrf24l01->spiHandle, nrf24l01->txBuff,
                                                           nrf24l01->payloadBuff, len+1);
    
    return status;
}

/** 
 * @brief When dma read is completed 
 */
void NRF24L01_Read_PayloadDMA_Complete(NRF24L01_STRUCT *nrf24l01, uint8_t *data, uint8_t len)
{
    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);
    nrf24l01->payloadFlag = 0;

    for (size_t i = 0; i < len; i++)
    {
        data[i] = nrf24l01->payloadBuff[i+1];
    }                               

}

/**
 * @brief Send payload with payload package
 * Call after selecting reading pipe
 */
HAL_StatusTypeDef NRF24L01_Write_ACKN_Payload(NRF24L01_STRUCT *nrf24l01, uint8_t *payload, uint8_t len){

    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_RESET);
    
    //ACK payload will be sent along with data arrival to selected reading pipe
    uint8_t tx_buff = (W_ACK_PAYLOAD | nrf24l01->pipeNum);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(nrf24l01->spiHandle, &tx_buff, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);
    if(status != HAL_OK){ return status; }
    
    status = HAL_SPI_Transmit(nrf24l01->spiHandle, payload, len, HAL_MAX_DELAY); 
    while(HAL_SPI_GetState(nrf24l01->spiHandle) != HAL_SPI_STATE_READY);      
    if(status != HAL_OK){ return status; }                                   

    HAL_GPIO_WritePin(nrf24l01->nrf24l01GpioPort, nrf24l01->csnPin, GPIO_PIN_SET);

    return status;

}

/** 
 * @brief Open reading pipe for rx data
 */
HAL_StatusTypeDef NRF24L01_Open_Reading_Pipe(NRF24L01_STRUCT *nrf24l01, uint8_t pipeAddr,
                                             uint64_t rxAddr, uint8_t payloadSize)
{   
    HAL_StatusTypeDef status; 
    // Check reading pipe
    if(pipeAddr == RX_ADDR_P0 || pipeAddr == RX_ADDR_P1){
        status = NRF24L01_Write(nrf24l01, pipeAddr, rxAddr, 5);
    }else{
        status = NRF24L01_Write_Byte(nrf24l01, pipeAddr, rxAddr);
    }
    if(status != HAL_OK){ return status; }                                    
    // Set payload size, add 0x07 to get a payload size register address
    status = NRF24L01_Write_Byte(nrf24l01, (pipeAddr + 0x07), payloadSize);
    if(status != HAL_OK){ return status; }                                    

    // Enable data pipe, subtract 0xa to get a value that points to the selected pipe
    // in the EN_RXADDR registry
    nrf24l01->pipeNum = (pipeAddr - 0xa);
    return NRF24L01_Write_Byte(nrf24l01, EN_RXADDR, 1<<(pipeAddr - 0xa));

}

/**
 * @brief Open writing pipe for tx data 
 */
HAL_StatusTypeDef NRF24L01_Open_Writing_Pipe(NRF24L01_STRUCT *nrf24l01, uint64_t txAddr)
{
    HAL_StatusTypeDef status = NRF24L01_Write(nrf24l01, RX_ADDR_P0, txAddr, 5);  
    if(status != HAL_OK){ return status; }

    return NRF24L01_Write(nrf24l01, TX_ADDR, txAddr, 5);
}

/**
 * @brief Enable Payload with ACKN package,
 * ACKN payloads are sent with TX addr equal to RX pipe addr.
 */
HAL_StatusTypeDef NRF24L01_Enable_ACKN_Payload(NRF24L01_STRUCT *nrf24l01)
{
    HAL_StatusTypeDef status = NRF24L01_Write_Byte(nrf24l01, DYNPD, (1<<DPL_P0|1<<DPL_P1)); 
    if(status != HAL_OK){ return status; }

    return NRF24L01_Write_Byte(nrf24l01, FEATURE, (1<<EN_ACK_PAY|1<<EN_DPL));
}

/**
 * @brief Read register data for debug 
 */
HAL_StatusTypeDef NRF24L01_Get_Info(NRF24L01_STRUCT *nrf24l01){
    uint8_t data = 0; uint64_t data_long = 0;

    HAL_StatusTypeDef status = NRF24L01_Read_Byte(nrf24l01, NRF_CONFIG, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, RF_SETUP, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, SETUP_AW, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read(nrf24l01, TX_ADDR, &data_long, 5);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, EN_AA, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, EN_RXADDR, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, DYNPD, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, RF_CH, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, NRF_STATUS, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, FIFO_STATUS, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, SETUP_RETR, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, OBSERVE_TX, &data);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read(nrf24l01, RX_ADDR_P0, &data_long, 5);
    if(status != HAL_OK){ return status; }

    status = NRF24L01_Read_Byte(nrf24l01, FEATURE, &data);
    if(status != HAL_OK){ return status; }

    return HAL_OK;
}
