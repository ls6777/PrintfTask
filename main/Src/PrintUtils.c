#include "PrintUtils.h"
#include <stdio.h>

#define MAX_LENGTH 15000
static char alignedBuffer[MAX_LENGTH];
static char* const buffer = (char*)(alignedBuffer);
static char* endBuffer = buffer + MAX_LENGTH;
static char* tail = buffer;
static char* head = buffer;
static int highWaterMark = 0;;

// function pointer that's used to flush the buffer when writing out from task
int (*realStdOutWrite)(struct _reent *, void *, const char *, int);

//------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------
/// @brief calculates the length currently used in the printf buffer
/// @return Size currently occupied in bytes
int CalculateBufferOccupiedSize();

/// @brief Function used by stdout for writing output from printf
/// @details This version is thread safe and protected with locks so a particular printf
///          call must finish before another start
/// @param[in] r - reentrancy info (unused)
/// @param[in] fd - file descriptor (unused)
/// @param[in] data - data to be written
/// @param[in] len - length of data to be written
/// @return len of data written to buffer
int WriteReroute(struct _reent* r, void* fd, const char* data, int len);

/// @brief Length of data to send from buffer to output
/// @return length of data to transmit
int CalculateLengthToOutput();

//------------------------------------------------------------------
// InitStdOut
//------------------------------------------------------------------
void InitStdOut()
{
    realStdOutWrite = _GLOBAL_REENT->_stdout->_write;
    _GLOBAL_REENT->_stdout->_write = WriteReroute;
}

//------------------------------------------------------------------
// WriteReroute
//------------------------------------------------------------------
int WriteReroute(struct _reent* r, void* fd, const char* data, int len)
{
    // check how much space is available, only output up to available space;
    int spaceRemaining = (MAX_LENGTH - 1) - CalculateBufferOccupiedSize();
    if (spaceRemaining < len)
    {
        len = spaceRemaining;
    }

    // put data into buffer
    for (int i = 0; i < len; i++)
    {
        *tail++ = *data++;
        if (tail == endBuffer)
        {
            tail = buffer;
        }
    }

    // update high water mark of buffer if needed. This can help developer determine appropriate size
    // for printf buffer
    int bufferOccupied = (MAX_LENGTH - 1) - (spaceRemaining - len);
    if (bufferOccupied > highWaterMark)
    {
        highWaterMark = bufferOccupied;
    }

    return len;
}

//------------------------------------------------------------------
// CalculateBufferOccupiedSize
//------------------------------------------------------------------
int CalculateBufferOccupiedSize()
{
    char* cHead = head;
    char* cTail = tail;

    int length = 0;
    if (cTail >= cHead)
    {
        length = cTail - cHead;
    }
    else
    {
        length = MAX_LENGTH - (cHead - cTail);
    }
    return length;
}

//------------------------------------------------------------------
// CalculateLengthToOutput
//------------------------------------------------------------------
int CalculateLengthToOutput()
{
    char* cHead = head;
    char* cTail = tail;
    int length = 0;

    if (cTail >= cHead)
    {
        length = cTail - cHead;
    }
    else
    {
        length = endBuffer - cHead;
    }
    return length;
}

//------------------------------------------------------------------
// TransmitData
//------------------------------------------------------------------
int TransmitData()
{
    int lengthToTransfer = CalculateLengthToOutput(head, tail);

    int dataTransfered = realStdOutWrite(_GLOBAL_REENT, _GLOBAL_REENT->_stdout, head, lengthToTransfer);

    head += dataTransfered;
    if (head >= endBuffer)
    {
        head = buffer;
    }

    return dataTransfered;
}

//------------------------------------------------------------------
// GetHighWaterMark
//------------------------------------------------------------------
int GetHighWaterMark()
{
    return highWaterMark;
}
