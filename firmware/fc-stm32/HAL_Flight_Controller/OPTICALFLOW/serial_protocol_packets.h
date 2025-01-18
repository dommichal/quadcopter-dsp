#ifndef SERIAL_PROTOCOL_PACKET_H_
#define SERIAL_PROTOCOL_PACKET_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SERIAL_PROTOCOL_START_BYTE 0x7E


typedef enum {
  /// Packet type for the connection request
  SERIAL_PROTOCOL_PACKET_TYPE_CONNECT = 0x01,
  /// Packet type for the disconnection request
  SERIAL_PROTOCOL_PACKET_TYPE_DISCONNECT = 0x02,
  /// Packet type for the PID controller data output
  SERIAL_PROTOCOL_PACKET_TYPE_CONTROL_OUTPUT = 0x03,
  /// Packet type for the PID controller data input
  SERIAL_PROTOCOL_PACKET_TYPE_CONTROL_INPUT = 0x04,
  /// Packet type for the micro vision telemetry data
  SERIAL_PROTOCOL_PACKET_TYPE_MICROVISION_TELEMETRY = 0x05,
  /// Packet type for the UAV telemetry data
  SERIAL_PROTOCOL_PACKET_TYPE_UAV_TELEMETRY = 0x06,
} SerialProtocol_PacketType;

/// Structure representing a packet header
typedef struct __attribute__((packed)) {
  uint8_t startByte;     ///< Start byte of the packet (always 0x7E)
  uint8_t packetType;    ///< Type of the packet
  uint8_t payloadLength; ///< Packet payload length
  uint16_t crc;          ///< Cyclic Redundancy Check (CRC) field
} SerialProtocol_PacketHeader;

static_assert(5 == sizeof(SerialProtocol_PacketHeader), "SerialProtocol_PacketHeader size is not 5 bytes");

enum {
  /// Maximum payload length
  SERIAL_PROTOCOL_MAX_PAYLOAD_LENGTH = 30,
  /// Maximum packet length
  SERIAL_PROTOCOL_MAX_PACKET_LENGTH = SERIAL_PROTOCOL_MAX_PAYLOAD_LENGTH + sizeof(SerialProtocol_PacketHeader)
};

/// Structure representing a packet
typedef struct __attribute__((packed)) {
  SerialProtocol_PacketHeader header;                  ///< Packet header
  uint8_t payload[SERIAL_PROTOCOL_MAX_PAYLOAD_LENGTH]; ///< Packet payload
} SerialProtocol_Packet;

static_assert(SERIAL_PROTOCOL_MAX_PACKET_LENGTH == sizeof(SerialProtocol_Packet), "SerialProtocol_Packet size is not 85 bytes");

typedef struct __attribute__((packed)) {
  uint8_t status;
  uint16_t connectionId;
} SerialProtocol_ConnectPayload;

static_assert(3 == sizeof(SerialProtocol_ConnectPayload), "SerialProtocol_ConnectPayload size is not 3 bytes");

typedef struct __attribute__((packed)) {
  uint8_t status;
  uint16_t connectionId;
} SerialProtocol_DisconnectPayload;

static_assert(3 == sizeof(SerialProtocol_DisconnectPayload), "SerialProtocol_DisconnectPayload size is not 3 bytes");

typedef struct __attribute__((packed)) {
  float thrustTarget;
  float rollTarget;
  float pitchTarget;
  float yawTarget;
} SerialProtocol_ControlOutputPayload;

static_assert(16 == sizeof(SerialProtocol_ControlOutputPayload), "SerialProtocol_ControlOutputPayload size is not 13 bytes");

typedef struct __attribute__((packed)) {
  int16_t velocityX;
  int16_t velocityY;
  int16_t velocityZ;
  float rollActual;
  float pitchActual;
  float yawActual;
  float angularVelocityX;
  float angularVelocityY;
  float angularVelocityZ;
} SerialProtocol_ControlInputPayload;

static_assert(30 == sizeof(SerialProtocol_ControlInputPayload), "SerialProtocol_ControlInputPayload size is not 13 bytes");

typedef struct __attribute__((packed)) {
  float elevation;
  float velocityX;
  float velocityY;
  float velocityZ;
  uint8_t status;
} SerialProtocol_MicrovisionTelemetryPayload;

static_assert(17 == sizeof(SerialProtocol_MicrovisionTelemetryPayload), "SerialProtocol_MicrovisionTelemetryPayload size is not 17 bytes");

typedef struct __attribute__((packed)) { 
  float roll;
  float pitch;
  float yaw;
  float angularVelocityX;
  float angularVelocityY;
  float angularVelocityZ;
  uint8_t status;
} SerialProtocol_UAVTelemetryPayload;

static_assert(25 == sizeof(SerialProtocol_UAVTelemetryPayload), "SerialProtocol_UAVTelemetryPayload size is not 29 bytes");

SerialProtocol_PacketType SerialProtocol_ReadPacketType(const uint8_t *sourceBuffer, size_t sourceBufferSize) __attribute__((nonnull));

size_t SerialProtocol_Serialize(uint8_t packetType, const void *payload, size_t payloadSize, SerialProtocol_Packet *packet) __attribute__((nonnull));

size_t SerialProtocol_Deserialize(const uint8_t *sourceBuffer, size_t sourceBufferSize, SerialProtocol_Packet *packet) __attribute__((nonnull));

#ifdef __cplusplus
}
#endif

#endif // SERIAL_PROTOCOL_PACKET_H_