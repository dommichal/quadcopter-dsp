#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "serial_protocol_packets.h"

#define RING_BUFFER_SIZE 3

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  SerialProtocol_Packet packet;
  size_t packetSize;
} SerialProtocol_PacketInfo;

typedef struct {
  SerialProtocol_PacketInfo buffer[RING_BUFFER_SIZE];
  size_t head;
  size_t tail;
  size_t count;
} SerialProtocol_RingBuffer;


void SerialProtocol_RingBuffer_Init(SerialProtocol_RingBuffer *ringBuffer);
bool SerialProtocol_RingBuffer_IsEmpty(const SerialProtocol_RingBuffer *ringBuffer) __attribute__((nonnull));
bool SerialProtocol_RingBuffer_IsFull(const SerialProtocol_RingBuffer *ringBuffer) __attribute__((nonnull));
bool SerialProtocol_RingBuffer_Enqueue(SerialProtocol_RingBuffer *ringBuffer, SerialProtocol_Packet *packet, size_t packetSize) __attribute__((nonnull));
bool SerialProtocol_RingBuffer_Dequeue(SerialProtocol_RingBuffer *ringBuffer, SerialProtocol_Packet *packet, size_t *packetSize) __attribute__((nonnull));

#ifdef __cplusplus
}
#endif
#endif // RING_BUFFER_H