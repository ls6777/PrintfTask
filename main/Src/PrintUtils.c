#include "PrintUtils.h"
#include <stdio.h>

#define MAX_LENGTH 15000
static char alignedBuffer[MAX_LENGTH];
static char* const buffer = (char*)(alignedBuffer);
static char* endBuffer = buffer + MAX_LENGTH;
static char* tail = buffer;
static char* head = buffer;
static int highWaterMark = 0;;

int (*realStdOutWrite)(struct _reent *, void *, const char *, int);
//__FILE* realStdOutWrite;

int CalculateBufferOccupiedSize();

void InitStdOut()
{
    realStdOutWrite = _GLOBAL_REENT->_stdout->_write;
    _GLOBAL_REENT->_stdout->_write = my_write;
}

////------------------------------------------------------------------
//// _write
////------------------------------------------------------------------
//int _write(int fd, char *ptr, int len)
//{
//    char* currHead = head;
//    if (len > (MAX_LENGTH - CalculateLength(currHead, tail)))
//    {
//        len = MAX_LENGTH - CalculateLength(currHead, tail) - 1;
//    }
//    for (int i = 0; i < len; ++i)
//    {
//        if (endBuffer == tail)
//        {
//            tail = buffer;
//        }
//        *tail++ = *(ptr + i);
//    }
//    return len;
//}

/// @brief Retargets the C library printf function to a circular buffer.
/// @param ch - character to output
/// @return character that was output
int my_write(struct _reent* r, void *fd,
                        const char *c,
                        int len)
{
//   return old_write(r, fd, c, len);
//    fileDesc = fd;
    int count = 0;
////    if (CalculateLength(head, tail) >= (MAX_LENGTH - 1))
////    {
////        return ch;
////    }
//    while (0 != *c)
    for (int i = 0; i < len; i++)
    {
        *tail++ = *c++;
        if (tail == endBuffer)
        {
            tail = buffer;
        }
        count++;
    }

    int bufferOccupied = CalculateBufferOccupiedSize();
    if (bufferOccupied > highWaterMark)
    {
        highWaterMark = bufferOccupied;
    }


    return count;
}

///// @brief Retargets the C library printf function to a circular buffer.
///// @param ch - character to output
///// @return character that was output
//int _write_r(struct _reent *ptr,
//        int fd,
//        const void *buf,
//        size_t cn)
//{
//    int count = 0;
////    if (CalculateLength(head, tail) >= (MAX_LENGTH - 1))
////    {
////        return ch;
////    }
//    while (0 != *(char*)(buf))
//    {
//        *tail++ = *(char*)(buf)++;
//        if (tail == endBuffer)
//        {
//            tail = 0;
//        }
//        count++;
//    }
//    return count;
//}

//------------------------------------------------------------------
// CalculateLength
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
// CalculateLengthToTransfer
//------------------------------------------------------------------
int CalculateLengthToTransfer(char* cHead, char* cTail)
{
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
    int lengthToTransfer = CalculateLengthToTransfer(head, tail);

    realStdOutWrite(_GLOBAL_REENT, _GLOBAL_REENT->_stdout, head, lengthToTransfer);

    head += lengthToTransfer;
    if (head >= endBuffer)
    {
        head = buffer;
    }
//    for (int i = 0; i < lengthToTransfer; i++)
//    {
//        if (endBuffer == head)
//        {
//            head = buffer;
//        }
////        putchar(*head++);
//        head++;
//    }

//    putchar('G');
//    putchar('\r');
//    putchar('\n');
    return lengthToTransfer;
}

int GetHighWaterMark()
{
    return highWaterMark;
}
