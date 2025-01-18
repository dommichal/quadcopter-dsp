#include "hal_radio.h"

#include "nrf24l01.h"

#include <assert.h>

typedef struct {
  NRF24L01_STRUCT nrf24l01;
  uint8_t telemetry[24];
  uint8_t payloadBuff[8];
  HAL_RADIO_receive_complete_callback_t radio_receive_callback;
  HAL_RADIO_request_receive_callback_t radio_request_receive_callback;
  bool telemetry_enabled;
} Radio_t;

static Radio_t radio;

void HAL_RADIO_init(
    HAL_RADIO_receive_complete_callback_t radio_receive_callback,
    HAL_RADIO_request_receive_callback_t radio_request_receive_callback) {
  assert(radio_receive_callback != NULL);

  // Initialize NRF24L01
  radio.nrf24l01.nrf24l01GpioPort = CE_GPIO_Port;
  radio.nrf24l01.csnPin = CSN_Pin;
  radio.nrf24l01.cePin = CE_Pin;
  radio.nrf24l01.spiHandle = &hspi1;
  
  radio.radio_receive_callback = radio_receive_callback;
  radio.radio_request_receive_callback = radio_request_receive_callback;
  radio.telemetry_enabled = true;

  NRF24L01_Init(&radio.nrf24l01, &nrf24l01_default_config);

  // Enable ACKN Payload for telemetry
  NRF24L01_Enable_ACKN_Payload(&radio.nrf24l01);
  const uint64_t rx_addr_pipe = 0xc2c2c2c2c2LL;
  NRF24L01_Open_Reading_Pipe(&radio.nrf24l01, RX_ADDR_P1, rx_addr_pipe, 8);

}

void HAL_RADIO_deinit() {}

void HAL_RADIO_proc() {}

void HAL_RADIO_enable_telemetry() { radio.telemetry_enabled = true; }

void HAL_RADIO_start_listening() {
  // Start listening for incoming messages
  NRF24L01_Start_Listening(&radio.nrf24l01);
}

void HAL_RADIO_request_readout() {
  NRF24L01_Stop_Listening(&radio.nrf24l01);
  radio.radio_request_receive_callback();
  if (radio.telemetry_enabled) {
    NRF24L01_Write_ACKN_Payload(&radio.nrf24l01, radio.telemetry, 24);
  }
  NRF24L01_Read_PayloadDMA(&radio.nrf24l01, 8);
}

void HAL_RADIO_receive_payload() {
  NRF24L01_Read_PayloadDMA_Complete(&radio.nrf24l01, radio.payloadBuff, 8);
  radio.radio_receive_callback(radio.payloadBuff, 8);
  NRF24L01_Start_Listening(&radio.nrf24l01);
}

void HAL_RADIO_write_telemetry_payload(const uint8_t *data, const uint8_t len) {
  assert(len <= 24);
  assert(data != NULL);
  memcpy(radio.telemetry, data, len);
}
