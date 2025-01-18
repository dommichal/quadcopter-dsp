#include "hal_opticalflow.h"

#include "serial_protocol.h"

#include "usart.h"

enum {
    HAL_OPTICALFLOW_PROCESS_INTERVAL = 10,
};

typedef enum {
    HAL_OPTICALFLOW_STATE_IDLE,
    HAL_OPTICALFLOW_STATE_CONNECTED,
} HAL_OPTICALFLOW_state_t;

static struct {
    SerialProtocol_UAV serial_protocol_uav;
    SerialProtocol_Packet tx_buffer;
    uint8_t rx_buffer[SERIAL_PROTOCOL_MAX_PACKET_LENGTH];
    size_t packetCount;
    size_t packetCountLast;
    size_t timeout;
    HAL_OPTICALFLOW_state_t state;
    HAL_OPTICALFLOW_timepoint_t last_process_time;
    HAL_OPTICALFLOW_conversion_complete_callback_t conversion_complete_callback;
} OPTICALFLOW_DEVICE;

static void onControlOutput(SerialProtocol_UAV *serialProtocol_uav, SerialProtocol_ControlOutputPayload const *controlOutput, void *context) {
    (void)serialProtocol_uav;
    (void)context;
    OPTICALFLOW_DEVICE.state = HAL_OPTICALFLOW_STATE_CONNECTED;
    HAL_OPTICALFLOW_conversion_result_t result = {
        .yaw = controlOutput->yawTarget,
        .pitch = controlOutput->pitchTarget,
        .roll = controlOutput->rollTarget,
        .thrust = controlOutput->thrustTarget,
    };
    ++OPTICALFLOW_DEVICE.packetCount;
    OPTICALFLOW_DEVICE.conversion_complete_callback(&result);
}

// static void onConnect(SerialProtocol *serialProtocol, SerialProtocol_ConnectPayload const *connect, void *context) {
//     (void)serialProtocol;
//     (void)connect;
//     (void)context;
// }

void HAL_OPTICALFLOW_init(HAL_OPTICALFLOW_conversion_complete_callback_t conversion_complete_callback) {
    OPTICALFLOW_DEVICE.conversion_complete_callback = conversion_complete_callback;
    OPTICALFLOW_DEVICE.state = HAL_OPTICALFLOW_STATE_IDLE;
    SerialProtocol_InitUAV(&OPTICALFLOW_DEVICE.serial_protocol_uav, NULL, NULL, onControlOutput, NULL, NULL);
}

void HAL_OPTICALFLOW_deinit(void) {}

void HAL_OPTICALFLOW_start_conversion(void) { HAL_UART_Receive_DMA(&huart1, OPTICALFLOW_DEVICE.rx_buffer, SERIAL_PROTOCOL_MAX_PACKET_LENGTH); }

void HAL_OPTICALFLOW_calculate(int16_t velocities[3], float angles[3], float angular_rates[3], HAL_OPTICALFLOW_timepoint_t now) {
    if (OPTICALFLOW_DEVICE.last_process_time + HAL_OPTICALFLOW_PROCESS_INTERVAL < now) {
        if (OPTICALFLOW_DEVICE.state != HAL_OPTICALFLOW_STATE_CONNECTED) {
            SerialProtocol_SendConnect(&OPTICALFLOW_DEVICE.serial_protocol_uav.serialProtocol);
        } else {
            if (OPTICALFLOW_DEVICE.packetCount > OPTICALFLOW_DEVICE.packetCountLast) {
                OPTICALFLOW_DEVICE.packetCountLast = OPTICALFLOW_DEVICE.packetCount;
                OPTICALFLOW_DEVICE.timeout > 0 ? --OPTICALFLOW_DEVICE.timeout : 0;
            } else {
                ++OPTICALFLOW_DEVICE.timeout;
                if (OPTICALFLOW_DEVICE.timeout > 100) {
                    OPTICALFLOW_DEVICE.state = HAL_OPTICALFLOW_STATE_IDLE;
                    OPTICALFLOW_DEVICE.timeout = 0;
                }
            }

            SerialProtocol_ControlInput controlInput = {
                .velocityX = velocities[0],
                .velocityY = velocities[1],
                .velocityZ = velocities[2],
                .rollActual = angles[0],
                .pitchActual = angles[1],
                .yawActual = angles[2],
                .angularVelocityX = angular_rates[0],
                .angularVelocityY = angular_rates[1],
                .angularVelocityZ = angular_rates[2],
            };
            SerialProtocol_SendUAVControlInput(&OPTICALFLOW_DEVICE.serial_protocol_uav, &controlInput);
        }
        size_t packetSize;
        SerialProtocol_Status status =
            SerialProtocol_GetPacketRedyToSend(&OPTICALFLOW_DEVICE.serial_protocol_uav.serialProtocol, &OPTICALFLOW_DEVICE.tx_buffer, &packetSize);
        if (status == SERIAL_PROTOCOL_STATUS_OK) {
            HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t *)&OPTICALFLOW_DEVICE.tx_buffer, SERIAL_PROTOCOL_MAX_PACKET_LENGTH, HAL_MAX_DELAY);
            if (status != HAL_OK) {
                Error_Handler();
            }
        }
        OPTICALFLOW_DEVICE.last_process_time = now;
    }
}

void HAL_OPTICALFLOW_readout(void) {
    SerialProtocol_UAVReceivePacket(&OPTICALFLOW_DEVICE.serial_protocol_uav, OPTICALFLOW_DEVICE.rx_buffer, SERIAL_PROTOCOL_MAX_PACKET_LENGTH);
    HAL_UART_Receive_DMA(&huart1, OPTICALFLOW_DEVICE.rx_buffer, SERIAL_PROTOCOL_MAX_PACKET_LENGTH);
}
