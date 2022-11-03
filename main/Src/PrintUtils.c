#include "PrintUtils.h"
#include <stdio.h>

#define MAX_LENGTH 15000
static char alignedBuffer[MAX_LENGTH];
static char* const buffer = (char*)(alignedBuffer);
static char* const endBuffer = buffer + MAX_LENGTH;
static char* tail = buffer;
static char* head = buffer;

//------------------------------------------------------------------
// _write
//------------------------------------------------------------------
int _write(int fd, char *ptr, int len)
{
    char* currHead = head;
    if (len > (MAX_LENGTH - CalculateLength(currHead, tail)))
    {
        len = MAX_LENGTH - CalculateLength(currHead, tail) - 1;
    }
    for (int i = 0; i < len; ++i)
    {
        if (endBuffer == tail)
        {
            tail = buffer;
        }
        *tail++ = *(ptr + i);
    }
    return len;
}

//------------------------------------------------------------------
// CalculateLength
//------------------------------------------------------------------
int CalculateLength(char* cHead, char* cTail)
{
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
// TransmitData
//------------------------------------------------------------------
void TransmitData()
{
    int lengthToTransfer = CalculateLength(head, tail);

    for (int i = 0; i < lengthToTransfer; i++)
    {
        if (endBuffer == head)
        {
            head = buffer;
        }
        putchar(*head++);
    }
}
