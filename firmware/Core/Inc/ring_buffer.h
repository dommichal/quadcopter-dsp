#ifndef _RING_BUFFER_
#define _RING_BUFFER_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** Structure describing the ring buffer. */
typedef struct {
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    uint32_t size;
    uint8_t *data;
} RingBuffer;

/**
 * Initializes the given ring buffer structure.
 *
 * @param ringBuffer pointer to a \ref RingBuffer structure
 * @param dataBuffer pointer to a location in memory, where the ring buffer data will be stored
 * @param dataBufferSize size in bytes of the dataBuffer
 * @return true if all arguments are valid and the ring buffer is initialized successfully, false otherwise
*/
bool RingBuffer_Init(RingBuffer *ringBuffer, char *dataBuffer, size_t dataBufferSize);

/**
 * Clears contents of the given ring buffer.
 *
 * @param ringBuffer pointer to a \ref RingBuffer structure
 * @return true if the ring buffer is cleared successfully, false otherwise
*/
bool RingBuffer_Clear(RingBuffer *ringBuffer);

/**
 * Checks if the given ring buffer is empty.
 *
 * @param ringBuffer pointer to a \ref RingBuffer structure
 * @return true if the ring buffer holds no data, false otherwise
*/
bool RingBuffer_IsEmpty(const RingBuffer *ringBuffer);

/**
 * Gets the length (in bytes) of the data stored in the given ring buffer.
 *
 * @param ringBuffer pointer to a \ref RingBuffer structure
 * @return length (in bytes) of the data stored in the ring buffer
*/
size_t RingBuffer_GetLen(const RingBuffer *ringBuffer);

/**
 * Returns the capacity (in bytes) of the given buffer.
 *
 * @param ringBuffer pointer to a \ref RingBuffer structure
 * @return capacity (in bytes) of the ring buffer (how much characters can it store)
*/
size_t RingBuffer_GetCapacity(const RingBuffer *ringBuffer);

/**
 * Appends a single character to the ring buffer. The stored data length will be
 * increased by 1.
 *
 * @param ringBuffer pointer to a \ref RingBuffer structure
 * @return true if the character was added successfully, false otherwise
*/
bool RingBuffer_PutChar(RingBuffer *ringBuffer, char c);

/**
 * Pulls out a single character from the ring buffer. The stored data length will be
 * decreased by 1.
 *
 * @param ringBuffer pointer to a \ref RingBuffer structure
 * @return true if the character was pulled out successfully, false otherwise
*/
bool RingBuffer_GetChar(RingBuffer *ringBuffer, char *c);


#endif //_RING_BUFFER_
