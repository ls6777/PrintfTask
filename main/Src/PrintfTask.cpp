#include "PrintfTask.hpp"
//#include "Message.hpp"
#include "Utils.hpp"
//#include <stdio.h>
#include "PrintUtils.h"

//QHandle PrintfTask::msgQHandle;
//
////------------------------------------------------------------------
//// Shutdown
////------------------------------------------------------------------
//void PrintfTask::Shutdown()
//{
//    Message msg(Message::SHUTDOWN);
//    ASSERT(PASS == QSend(msgQHandle, &msg));
//}

////------------------------------------------------------------------
//// Initialize
////------------------------------------------------------------------
//void PrintfTask::Initialize()
//{
//    msgQHandle = CreateQ(10, sizeof(Message));
//    Message msg(Message::INITIALIZE);
//    ASSERT(PASS == QSend(msgQHandle, &msg));
//}

//------------------------------------------------------------------
// Run
//------------------------------------------------------------------
void PrintfTask::Run()
{
    while (true)
    {
        // Print out any data in the buffer
        TransmitData();

        DELAY_MS(10);
    }
}

////------------------------------------------------------------------
//// Printf
////------------------------------------------------------------------
//void PrintfTask::Printf(const char *format, ...)
//{
//    uint32_t size1 = 0;
//    uint32_t size2 = 0;
//    char* currHead = head;
//    char* currTail = tail;
//
//    // Check if room for more data in the buffer
//
//    if (currTail > currHead)
//    {
//        size1 = (MAX_LENGTH + buffer) - currTail;
//    }
//    else
//    {
//        size1 = currHead - currTail - 1;
//    }
//    va_list args;
//    va_start(args, format);
//    uint32_t rtnSize = vsnprintf(currTail, size1, format, args);
//    if (rtnSize > size1)
//    {
//
//    }
//    va_end(args);
////    fflush(stdout);
////    calculate_high_water_mark();
//}

////------------------------------------------------------------------
//// TransmitData
////------------------------------------------------------------------
//void PrintfTask::TransmitData()
//{
//    uint32_t writeSize = 0;
//    char* currHead = head;
//    char* currTail = tail;

//    if (currHead == currTail)
//    {
//        return; // nothing to transmit
//    }
//    if (currTail > currHead)
//    {
//        writeSize = currTail - currHead;
//    }
//    else
//    {
//        writeSize = (MAX_LENGTH + buffer) - currHead;
//    }
//
//    // Transmit data out
//    for (uint32_t i = 0; i < writeSize; i++)
//    {
//        putchar(*(currHead + i));
//    }

//    if (currTail > currHead)
//    {
//        head = currTail;
//    }
//    else
//    {
//        head = buffer;
//    }
//}

