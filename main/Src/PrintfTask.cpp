#include "PrintfTask.hpp"
#include "Utils.hpp"
#include "PrintUtils.h"

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
