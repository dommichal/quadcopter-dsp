/**
 * @file radio.h
 * @author Dominik Michalczyk
 *
 * Radio module platform independent interface.
 */

#ifndef RADIO_H_
#define RADIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define HAL_RADIO_INTERFACE_SPI

typedef void (*HAL_RADIO_receive_complete_callback_t)(const uint8_t *data,
                                                      uint8_t len);
typedef void (*HAL_RADIO_request_receive_callback_t)(void);

void HAL_RADIO_init(
    HAL_RADIO_receive_complete_callback_t radio_receive_callback,
    HAL_RADIO_request_receive_callback_t radio_request_receive_callback);
void HAL_RADIO_deinit(void);
void HAL_RADIO_proc(void);
void HAL_RADIO_enable_telemetry(void);
void HAL_RADIO_start_listening(void);
void HAL_RADIO_request_readout(void);
void HAL_RADIO_receive_payload(void);
void HAL_RADIO_write_telemetry_payload(
    const uint8_t *data, const uint8_t len); // TODO: ADD CIRCULAR BUFFER

#ifdef __cplusplus
}
#endif

#endif // RADIO_H_
