#include "PrintfTask.hpp"
#include "Utils.hpp"
#include "PrintUtils.h"

extern "C" int (*old_write)(struct _reent *, void *, const char *, int);
//------------------------------------------------------------------
// Run
//------------------------------------------------------------------
void PrintfTask::Run()
{
//    int maxLength = 0;

    InitStdOut();
//    __FILE* realStdOut = _GLOBAL_REENT->_stdout;
//    old_write = _GLOBAL_REENT->_stdout->_write;

//    _GLOBAL_REENT->_stdout->_write = my_write;
    while (true)
    {
        // Print out any data in the buffer
        TransmitData();
//        realStdOut->write(_GLOBAL_REENT, realStdOut, )
//        int length = CalculateLength();
//        if (length > maxLength)
//        {
//            maxLength = length;
//            printf("Max Length: %d\r\n", maxLength);
//        }
        DELAY_MS(10);
    }
}
