#include "serial_protocol_packets.h"

#include <string.h>

// // TODO: replace with hardware implementation
static uint16_t SerialProtocol_UpdateCRC(uint8_t byte, uint16_t crc)
{
	byte ^= (uint8_t)(crc & 0x00ff);
	byte ^= (uint8_t)(byte << 4);
	return ((((uint16_t)byte << 8) | (uint8_t)(crc >> 8)) ^ (uint8_t)(byte >> 4) ^ ((uint16_t)byte << 3));
}

 SerialProtocol_PacketType SerialProtocol_ReadPacketType(const uint8_t *sourceBuffer, size_t sourceBufferSize) {
  if (sourceBufferSize < sizeof(SerialProtocol_Packet)) {
    return 0;
  }

  SerialProtocol_PacketHeader header;
  memcpy(&header, sourceBuffer, sizeof(header));
  return header.packetType;
}

size_t SerialProtocol_Serialize(uint8_t packetType, const void *payload, size_t payloadSize, SerialProtocol_Packet *packet) { 
  if (payloadSize > SERIAL_PROTOCOL_MAX_PAYLOAD_LENGTH){
    return 0;
  }

  // // TODO: Calculate CRC

  SerialProtocol_PacketHeader *header = &packet->header;
  header->startByte = SERIAL_PROTOCOL_START_BYTE;
  header->packetType = packetType;
  header->payloadLength = payloadSize;
  header->crc = 0;

  memcpy(packet->payload, payload, payloadSize);


  return sizeof(SerialProtocol_PacketHeader) + payloadSize;
}

size_t SerialProtocol_Deserialize(const uint8_t *sourceBuffer, size_t sourceBufferSize, SerialProtocol_Packet *packet) {
  if (sourceBufferSize < sizeof(SerialProtocol_PacketHeader)) {
    return 0;
  }

  SerialProtocol_PacketHeader header;
  memcpy(&header, sourceBuffer, sizeof(header));
  if(header.startByte != SERIAL_PROTOCOL_START_BYTE && header.payloadLength > SERIAL_PROTOCOL_MAX_PAYLOAD_LENGTH){
    return 0;
  }

  memcpy(packet, sourceBuffer, sizeof(*packet)); 
  return packet->header.payloadLength;
}
