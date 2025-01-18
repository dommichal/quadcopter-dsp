#include "serial_protocol_ring_buffer.h"

#include <assert.h>
#include <string.h>

void SerialProtocol_RingBuffer_Init(SerialProtocol_RingBuffer *ringBuffer) {
  assert(ringBuffer != NULL);

  ringBuffer->head = 0;
  ringBuffer->tail = 0;
  ringBuffer->count = 0;
}

bool SerialProtocol_RingBuffer_IsEmpty(const SerialProtocol_RingBuffer *ringBuffer) { return ringBuffer->count == 0; }

bool SerialProtocol_RingBuffer_IsFull(const SerialProtocol_RingBuffer *ringBuffer) { return ringBuffer->count == RING_BUFFER_SIZE; }

bool SerialProtocol_RingBuffer_Enqueue(SerialProtocol_RingBuffer *ringBuffer, SerialProtocol_Packet *packet, size_t packetSize) {
  if (SerialProtocol_RingBuffer_IsFull(ringBuffer) || packetSize > sizeof(SerialProtocol_Packet)) {
    return false;
  }

  SerialProtocol_PacketInfo *packetInfo = &ringBuffer->buffer[ringBuffer->head];
  memcpy(&packetInfo->packet, packet, packetSize);
  packetInfo->packetSize = packetSize;
  
  ringBuffer->head = (ringBuffer->head + 1) % RING_BUFFER_SIZE;
  ringBuffer->count++;
  return true;
}

bool SerialProtocol_RingBuffer_Dequeue(SerialProtocol_RingBuffer *ringBuffer, SerialProtocol_Packet *packet, size_t *packetSize) {
  if (SerialProtocol_RingBuffer_IsEmpty(ringBuffer)) {
    return false;
  }

  SerialProtocol_PacketInfo *packetInfo = &ringBuffer->buffer[ringBuffer->tail];
  memcpy(packet, &packetInfo->packet, packetInfo->packetSize);
  *packetSize = packetInfo->packetSize;

  ringBuffer->tail = (ringBuffer->tail + 1) % RING_BUFFER_SIZE;
  ringBuffer->count--;
  return true;
}
