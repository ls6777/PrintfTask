/// @file PrintUtils.c
/// @brief Target independent stdout API (see PrintUtils.h).

#include "PrintUtils.h"
#include "PrintBuffer.h"
#include "StdOutPort.h"

//------------------------------------------------------------------
// InitStdOut
//------------------------------------------------------------------
void InitStdOut(void)
{
    StdOutPort_Redirect();
}

//------------------------------------------------------------------
// TransmitData
//------------------------------------------------------------------
int TransmitData(void)
{
    return PrintBuffer_Transmit(StdOutPort_Write);
}

//------------------------------------------------------------------
// GetHighWaterMark
//------------------------------------------------------------------
int GetHighWaterMark(void)
{
    return PrintBuffer_GetHighWaterMark();
}
