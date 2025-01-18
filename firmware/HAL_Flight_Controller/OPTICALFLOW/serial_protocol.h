#ifndef SERIAL_PROTOCOL_H_
#define SERIAL_PROTOCOL_H_

#include "serial_protocol_packets.h"
#include "serial_protocol_ring_buffer.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SERIAL_PROTOCOL_STATUS_OK = 0x00,
  SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT = 0x01,
  SERIAL_PROTOCOL_STATUS_INVALID_PACKET = 0x02,
  SERIAL_PROTOCOL_STATUS_CRC_ERROR = 0x03,
  SERIAL_PROTOCOL_STATUS_BUFFER_FULL = 0x04,
  SERIAL_PROTOCOL_STATUS_BUFFER_EMPTY = 0x05,
} SerialProtocol_Status;

typedef enum {
  SERIAL_PROTOCOL_STATE_WAITING,
  SERIAL_PROTOCOL_STATE_CONNECTED,
  SERIAL_PROTOCOL_STATE_DISCONNECTED,
} SerialProtocol_State;

typedef struct SerialProtocol_StatusInfo {
  bool systemStatus;
} SerialProtocol_StatusInfo;

typedef struct SerialProtocol_ConnectionInfo {
  uint8_t status;
  uint16_t connectionId;
} SerialProtocol_ConnectionInfo;

typedef struct SerialProtocol_ControlInput {
  int16_t velocityX;
  int16_t velocityY;
  int16_t velocityZ;
  float rollActual;
  float pitchActual;
  float yawActual;
  float angularVelocityX;
  float angularVelocityY;
  float angularVelocityZ;
} SerialProtocol_ControlInput;

typedef struct SerialProtocol_ControlOutput {
  float thrustTarget;
  float rollTarget;
  float pitchTarget;
  float yawTarget;
} SerialProtocol_ControlOutput;

typedef struct SerialProtocol_MicrovisionTelemetry {
  float elevation;
  float velocityX;
  float velocityY;
  float velocityZ;
  SerialProtocol_StatusInfo status;
} SerialProtocol_MicrovisionTelemetry;

typedef struct SerialProtocol_UAVTelemetry { // todo
  float roll;
  float pitch;
  float yaw;
  float angularVelocityX;
  float angularVelocityY;
  float angularVelocityZ;
  SerialProtocol_StatusInfo status;
} SerialProtocol_UAVTelemetry;

/// Forward declaration
typedef struct SerialProtocol SerialProtocol;

/// Forward declaration
typedef struct SerialProtocol_UAV SerialProtocol_UAV;

/// Forward declaration
typedef struct SeiralProtocol_Microvision SeiralProtocol_Microvision;

typedef void (*SerialProtocol_OnConnect)(SerialProtocol *serialProtocol, SerialProtocol_ConnectPayload const *connect, void *context);

typedef void (*SerialProtocol_OnDisconnect)(SerialProtocol *serialProtocol, SerialProtocol_DisconnectPayload const *disconnect, void *context);

typedef void (*SerialProtocol_OnControlInput)(SeiralProtocol_Microvision *serialProtocol, SerialProtocol_ControlInputPayload const *controlInput,
                                              void *context);

typedef void (*SerialProtocol_OnControlOutput)(SerialProtocol_UAV *serialProtocol, SerialProtocol_ControlOutputPayload const *controlOutput,
                                               void *context);

typedef void (*SerialProtocol_OnMicrovisionTelemetry)(SerialProtocol_UAV *serialProtocol, SerialProtocol_MicrovisionTelemetryPayload const *telemetry,
                                                      void *context);

typedef void (*SerialProtocol_OnUAVTelemetry)(SeiralProtocol_Microvision *serialProtocol, SerialProtocol_UAVTelemetryPayload const *telemetry,
                                              void *context);

typedef struct SerialProtocol {
  SerialProtocol_Packet rxBuff;
  SerialProtocol_RingBuffer txBuff;
  SerialProtocol_State state;
  SerialProtocol_ConnectionInfo connectionInfo;
  SerialProtocol_OnConnect onConnect;
  SerialProtocol_OnDisconnect onDisconnect;
  void *context;
} SerialProtocol;

typedef struct SerialProtocol_UAV {
  SerialProtocol serialProtocol;
  SerialProtocol_OnControlOutput onControlOutput;
  SerialProtocol_OnMicrovisionTelemetry onMicrovisionTelemetry;
} SerialProtocol_UAV;

typedef struct SeiralProtocol_Microvision {
  SerialProtocol serialProtocol;
  SerialProtocol_OnControlInput onControlInput;
  SerialProtocol_OnUAVTelemetry onUAVTelemetry;
} SeiralProtocol_Microvision;

// /**
//  * Initialize the serial protocol.
//  *
//  * @param[in] serialProtocol Serial protocol instance
//  *
//  * @return SerialProtocol_Status
//  */
// SerialProtocol_Status SerialProtocol_Init(SerialProtocol *serialProtocol);

SerialProtocol_Status SerialProtocol_InitUAV(SerialProtocol_UAV *serialProtocolUAV, SerialProtocol_OnConnect onConnect,
                                             SerialProtocol_OnDisconnect onDisconnect, SerialProtocol_OnControlOutput onControlOutput,
                                             SerialProtocol_OnMicrovisionTelemetry onMicrovisionTelemetry, void *context);

SerialProtocol_Status SerialProtocol_InitMicrovision(SeiralProtocol_Microvision *serialProtocolMicrovision, SerialProtocol_OnConnect onConnect,
                                                     SerialProtocol_OnDisconnect onDisconnect, SerialProtocol_OnControlInput onControlInput,
                                                     SerialProtocol_OnUAVTelemetry onUAVTelemetry, void *context);

/**
 * Gets a packet ready to be sent.
 *
 * @param[in] serialProtocol Serial protocol instance
 *
 * @param[out] packetToSend pointer to the packet ready to be sent or NULL if no packet is ready
 */
SerialProtocol_Status SerialProtocol_GetPacketRedyToSend(SerialProtocol *serialProtocol, SerialProtocol_Packet *packetToSend, size_t *packetSize);

SerialProtocol_Status SerialProtocol_UAVReceivePacket(SerialProtocol_UAV *serialProtocolUAV, uint8_t *sourceBuffer, size_t sourceBufferSize);

SerialProtocol_Status SerialProtocol_MicrovisionReceivePacket(SeiralProtocol_Microvision *serialProtocolMicrovision, uint8_t *sourceBuffer,
                                                              size_t sourceBufferSize);

SerialProtocol_Status SerialProtocol_SendConnect(SerialProtocol *serialProtocol);

SerialProtocol_Status SerialProtocol_SendDisconnect(SerialProtocol *serialProtocol);

SerialProtocol_Status SerialProtocol_SendUAVControlInput(SerialProtocol_UAV *serialProtocolUAV, SerialProtocol_ControlInput const *controlInput);

SerialProtocol_Status SerialProtocol_SendUAVTelemetry(SerialProtocol_UAV *serialProtocolUAV, SerialProtocol_UAVTelemetry const *telemetry);

SerialProtocol_Status SerialProtocol_SendMicrovisionControlOutput(SeiralProtocol_Microvision *serialProtocolMicrovision,
                                                                  SerialProtocol_ControlOutput const *controlOutput);

SerialProtocol_Status SerialProtocol_SendMicrovisionTelemetry(SeiralProtocol_Microvision *serialProtocolMicrovision,
                                                              SerialProtocol_MicrovisionTelemetry const *telemetry);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_PROTOCOL_H_
