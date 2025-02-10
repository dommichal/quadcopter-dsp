/* Includes ------------------------------------------------------------------*/
#include "ring_buffer.h"

#include <assert.h>
#include <stdio.h>

bool RingBuffer_Init(RingBuffer *ringBuffer, char *dataBuffer, size_t dataBufferSize) {
    assert(ringBuffer);
    assert(dataBuffer);
    assert(dataBufferSize > 0);

    if ((ringBuffer) && (dataBuffer) && (dataBufferSize > 0)) {
        ringBuffer->size = dataBufferSize;
        ringBuffer->data = dataBuffer;
        ringBuffer->count = 0;
        ringBuffer->tail = 0;
        ringBuffer->head = 0;
        return true;
    }

    return false;
}

bool RingBuffer_Clear(RingBuffer *ringBuffer) {
    assert(ringBuffer);

    if (ringBuffer) {
        for (size_t i; i < ringBuffer->size; i++) {
            ringBuffer->data[i] = 0x00;
        }
        ringBuffer->count = 0;
        ringBuffer->tail = 0;
        ringBuffer->head = 0;
        return true;
    }
    return false;
}

bool RingBuffer_IsEmpty(const RingBuffer *ringBuffer) {
    assert(ringBuffer);

    return (ringBuffer->count == 0);
}

size_t RingBuffer_GetLen(const RingBuffer *ringBuffer) {
    assert(ringBuffer);

    if (ringBuffer) {
        return ringBuffer->count;
    }
    return 0;
}

size_t RingBuffer_GetCapacity(const RingBuffer *ringBuffer) {
    assert(ringBuffer);

    if (ringBuffer) {
        return ringBuffer->size;
    }
    return 0;
}

bool RingBuffer_PutChar(RingBuffer *ringBuffer, char c) {
    assert(ringBuffer);

    if (ringBuffer) {
        // check if buffer is full
        if (ringBuffer->size - ringBuffer->count > 0) {
            ringBuffer->data[ringBuffer->head] = c;
            ringBuffer->head = (ringBuffer->head + 1) % ringBuffer->size;
            ringBuffer->count++;
            return true;
        }
    }
    return false;
}

bool RingBuffer_GetChar(RingBuffer *ringBuffer, char *c) {
    assert(ringBuffer);
    assert(c);

    if ((ringBuffer) && (c)) {
        // check if buffer is empty
        if (!RingBuffer_IsEmpty(ringBuffer)) {
            *c = ringBuffer->data[ringBuffer->tail];
            ringBuffer->tail = (ringBuffer->tail + 1) % ringBuffer->size;
            ringBuffer->count--;
            return true;
        }
    }
    return false;
}
