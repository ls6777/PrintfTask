#include "PrintBuffer.h"

#include <stddef.h>
#include <string.h>

#define BUFFER_SIZE ((int)PRINTF_BUFFER_SIZE)

static char buffer[BUFFER_SIZE];

// head: next byte to output (only modified by the consumer, PrintBuffer_Transmit)
// tail: next free byte to write (only modified by the producer, PrintBuffer_Write)
static int head = 0;
static int tail = 0;
static int highWaterMark = 0;

//------------------------------------------------------------------
// OccupiedSize
//------------------------------------------------------------------
static int OccupiedSize(int cHead, int cTail)
{
    return (cTail >= cHead) ? (cTail - cHead) : (BUFFER_SIZE - (cHead - cTail));
}

//------------------------------------------------------------------
// PrintBuffer_Write
//------------------------------------------------------------------
int PrintBuffer_Write(const char* data, int len)
{
    if ((data == NULL) || (len <= 0))
    {
        return 0;
    }

    const int cHead = head;
    const int cTail = tail;

    // only write up to the available space, the rest is dropped
    const int spaceRemaining = (BUFFER_SIZE - 1) - OccupiedSize(cHead, cTail);
    if (len > spaceRemaining)
    {
        len = spaceRemaining;
    }

    // copy in up to two pieces: up to the end of the buffer, then any remainder at the start
    const int firstLen = (len < BUFFER_SIZE - cTail) ? len : (BUFFER_SIZE - cTail);
    memcpy(&buffer[cTail], data, (size_t)firstLen);
    memcpy(&buffer[0], data + firstLen, (size_t)(len - firstLen));

    tail = (cTail + len) % BUFFER_SIZE;

    // track the high water mark so the developer can determine an appropriate buffer size
    const int occupied = OccupiedSize(cHead, tail);
    if (occupied > highWaterMark)
    {
        highWaterMark = occupied;
    }

    return len;
}

//------------------------------------------------------------------
// PrintBuffer_Transmit
//------------------------------------------------------------------
int PrintBuffer_Transmit(PrintBufferOutputFn output)
{
    int total = 0;

    // at most two passes: data up to the end of the buffer, then data that wrapped to the start
    for (int pass = 0; pass < 2; pass++)
    {
        const int cHead = head;
        const int cTail = tail;
        const int contiguousLen = (cTail >= cHead) ? (cTail - cHead) : (BUFFER_SIZE - cHead);

        if (contiguousLen == 0)
        {
            break;
        }

        int written = output(&buffer[cHead], contiguousLen);
        if (written < 0)
        {
            return written;
        }
        if (written > contiguousLen)
        {
            written = contiguousLen;
        }

        head = (cHead + written) % BUFFER_SIZE;
        total += written;

        if (written < contiguousLen)
        {
            break;
        }
    }

    return total;
}

//------------------------------------------------------------------
// PrintBuffer_GetOccupiedSize
//------------------------------------------------------------------
int PrintBuffer_GetOccupiedSize(void)
{
    return OccupiedSize(head, tail);
}

//------------------------------------------------------------------
// PrintBuffer_GetHighWaterMark
//------------------------------------------------------------------
int PrintBuffer_GetHighWaterMark(void)
{
    return highWaterMark;
}

//------------------------------------------------------------------
// PrintBuffer_Reset
//------------------------------------------------------------------
void PrintBuffer_Reset(void)
{
    head = 0;
    tail = 0;
    highWaterMark = 0;
}
