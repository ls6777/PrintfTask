#include "PrintfTask.hpp"
#include "Utils.hpp"
#include "PrintUtils.h"

//------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------
void PrintfTask::Initialize()
{
    InitStdOut();
}

//------------------------------------------------------------------
// Run
//------------------------------------------------------------------
void PrintfTask::Run()
{
    while (true)
    {
        // Print out any data in the buffer
        TransmitData();

        // This can be any value that you want to delay for other lower priority tasks, or sleeping.
        // If this is the lowest priority task, it's possible to remove a delay completely in which
        // case the Transmit will continuously poll for new data and print out when this task is active.
        DELAY_MS(10);
    }
}
