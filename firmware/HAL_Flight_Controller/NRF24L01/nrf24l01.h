#if !defined(NRF24L01)
#define NRF24L01

#include "gpio.h"
#include "spi.h"
#include "tim.h"

#include <string.h>
#include <stdint.h>

/* RF Power Select Enum */
typedef enum {
    RF_POWER_0 = 0x00U, //-18dBm
    RF_POWER_1 = 0x01U, //-12dBm
    RF_POWER_2 = 0x02U, //-6dBm
    RF_POWER_3 = 0x03U  // 0dBm
} RF_POWER_SEL;

/* Address filed width Enum */
typedef enum { THREE_BYTES_ADDR = 0x01U, FOUR_BYTES_ADDR = 0x02U, FIVE_BYTES_ADDR = 0x03U } RX_TX_ADDRESS_WIDTH;

/* Air data Rate Enum */
typedef enum {
    ONE_MBPS_DATA_RATE = 0x00U, // 1Mbps
    TWO_MBPS_DATA_RATE = 0x01U  // 2Mbps
} AIR_DATA_RATE;

/* RX_DR INTERRUPT ENUM */
typedef enum { RX_DR_INT_SET = 0x00U, RX_DR_INT_RESET = 0x01U } NRF_RX_INTERRUPTS;

/* TX_DS INTERRUPT ENUM */
typedef enum { TX_DS_INT_SET = 0x00U, TX_DS_INT_RESET = 0x01U } NRF_TX_INTERRUPTS;

/* MAX_RT INTERRUPT ENUM */
typedef enum { MAX_RT_INT_SET = 0x00U, MAX_RT_INT_RESET = 0x01U } MAX_RT_INTERRUPTS;

/* CRCO ENCODING SCHEME ENUM  */
typedef enum { ONE_BYTE_ENCODING = 0X00U, TWO_BYTES_ENCODING = 0X04U } CRCO_ENCODING;

/* EN_CRC  ENUM */
typedef enum { EN_CRC_RESET = 0x00U, EN_CRC_SET = 0x01U } EN_CRC;

/* RX/TX Mode Enum */
typedef enum { PTX = 0x00U, PRX = 0x01U } RX_TX_CONTROL;

/* LNA_HCURR Enum */
typedef enum { LNA_HCURR_RESET = 0x00U, LNA_HCURR_SET = 0x01U } LNA_HCURR;

/* Transceiver struct */
typedef struct {
    SPI_HandleTypeDef *spiHandle;

    GPIO_TypeDef *nrf24l01GpioPort;
    uint16_t csnPin;
    uint16_t cePin;

    /* DMA */
    uint8_t payloadBuff[33];
    uint8_t txBuff[33];

    uint8_t payloadFlag;

    /* PIPE Addr */
    uint8_t pipeNum;

} NRF24L01_STRUCT;

/* Transceiver Init Struct */
typedef struct {
    NRF_RX_INTERRUPTS rx_int;
    NRF_TX_INTERRUPTS tx_int;
    MAX_RT_INTERRUPTS max_rt_int;

    EN_CRC en_crc;
    CRCO_ENCODING crco_enc;
    RX_TX_CONTROL rx_tx_control;
    RX_TX_ADDRESS_WIDTH rx_tx_addr_width;
    LNA_HCURR lna_hcurr;
    AIR_DATA_RATE data_rate;
    RF_POWER_SEL power_sel;

    uint8_t re_transmission_delay;
    uint8_t re_transmission_num;
    uint8_t chanel;
} NRF24L01_CONFIG;

/* Default NRF24L01 Config */
extern NRF24L01_CONFIG nrf24l01_default_config;

/* LOW LEVEL FUNCTIONS */
HAL_StatusTypeDef NRF24L01_Read_Byte(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint8_t *data);

HAL_StatusTypeDef NRF24L01_Write_Byte(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint8_t data);

HAL_StatusTypeDef NRF24L01_Read(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint64_t *data, uint8_t len);

HAL_StatusTypeDef NRF24L01_Write(NRF24L01_STRUCT *nrf24l01, uint8_t addr, uint64_t data, uint8_t len);

HAL_StatusTypeDef NRF24L01_Flush_Tx(NRF24L01_STRUCT *nrf24l01);

HAL_StatusTypeDef NRF24L01_Flush_Rx(NRF24L01_STRUCT *nrf24l01);
/* END OF LOW LEVEL FUNCTIONS */

/* MAIN FUNCTIONS */
HAL_StatusTypeDef NRF24L01_Init(NRF24L01_STRUCT *nrf24l01, NRF24L01_CONFIG *nrf24l01_cfg);

HAL_StatusTypeDef NRF24L01_Chanel(NRF24L01_STRUCT *nrf24l01, uint8_t rfCh);

HAL_StatusTypeDef NRF24L01_Open_Writing_Pipe(NRF24L01_STRUCT *nrf24l01, uint64_t txAddr);

HAL_StatusTypeDef NRF24L01_Enable_ACKN_Payload(NRF24L01_STRUCT *nrf24l01);

HAL_StatusTypeDef NRF24L01_Write_Tx_Payload(NRF24L01_STRUCT *nrf24l01, void *payload, uint8_t len);

HAL_StatusTypeDef NRF24L01_Send_Payload(NRF24L01_STRUCT *nrf24l01);

HAL_StatusTypeDef NRF24L01_Send(NRF24L01_STRUCT *nrf24l01, void *data, uint8_t len);

HAL_StatusTypeDef NRF24L01_Open_Reading_Pipe(NRF24L01_STRUCT *nrf24l01, uint8_t pipeAddr, uint64_t rxAddr, uint8_t payloadSize);

HAL_StatusTypeDef NRF24L01_Packet_Available(NRF24L01_STRUCT *nrf24l01);

HAL_StatusTypeDef NRF24L01_Start_Listening(NRF24L01_STRUCT *nrf24l01);

void NRF24L01_Stop_Listening(NRF24L01_STRUCT *nrf24l01);

HAL_StatusTypeDef NRF24L01_Read_Payload(NRF24L01_STRUCT *nrf24l01, uint8_t *data, uint8_t len);

HAL_StatusTypeDef NRF24L01_Get_Info(NRF24L01_STRUCT *nrf24l01);

HAL_StatusTypeDef NRF24L01_Read_PayloadDMA(NRF24L01_STRUCT *nrf24l01, uint8_t len);

void NRF24L01_Read_PayloadDMA_Complete(NRF24L01_STRUCT *nrf24l01, uint8_t *data, uint8_t len);

HAL_StatusTypeDef NRF24L01_Write_ACKN_Payload(NRF24L01_STRUCT *nrf24l01, uint8_t *data, uint8_t len);
/* END OF MAIN FUNCTIONS */

/* Memory Map */
#define NRF_CONFIG 0x00
#define EN_AA 0x01
#define EN_RXADDR 0x02
#define SETUP_AW 0x03
#define SETUP_RETR 0x04
#define RF_CH 0x05
#define RF_SETUP 0x06
#define NRF_STATUS 0x07
#define OBSERVE_TX 0x08
#define CD 0x09
#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17
#define DYNPD 0x1C
#define FEATURE 0x1D

/* Bit Mnemonics */
#define MASK_RX_DR 6
#define MASK_TX_DS 5
#define MASK_MAX_RT 4
#define EN_CRC_MASK 3
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0
#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0
#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0
#define AW 0
#define ARD 4
#define ARC 0
#define PLL_LOCK 4
#define CONT_WAVE 7
#define RF_DR 3
#define RF_PWR 1
#define RX_P_NO 1
#define TX_FULL 0
#define PLOS_CNT 4
#define ARC_CNT 0
#define TX_REUSE 6
#define FIFO_FULL 5
#define TX_EMPTY 4
#define RX_FULL 1
#define RX_EMPTY 0
#define DPL_P5 5
#define DPL_P4 4
#define DPL_P3 3
#define DPL_P2 2
#define DPL_P1 1
#define DPL_P0 0
#define EN_DPL 2
#define EN_ACK_PAY 1
#define EN_DYN_ACK 0

/* Instruction Mnemonics */
#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE 0x50
#define R_RX_PL_WID 0x60
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define RF24_NOP 0xFF

/* P model memory Map */
#define RPD 0x09
#define W_TX_PAYLOAD_NO_ACK 0xB0

/* P model bit Mnemonics */
#define RF_DR_LOW 0
#define RF_DR_HIGH 8

/* Other */
#define MAX_RX_PAYLOAD_SIZE 33
#define MAX_RF_CH 127
#define POWER_ON_TIME 105

#endif // NRF24L01
