/// @file TargetPort.cpp
/// @brief OS abstraction layer for FreeRTOS (see TargetPort.hpp).

#include "TargetPort.hpp"

//------------------------------------------------------------------
// CreateQ
//------------------------------------------------------------------
QHandle TargetPort::CreateQ(uint32_t qLength, uint32_t itemSize)
{
    return xQueueCreate(qLength, itemSize);
}

//------------------------------------------------------------------
// QSend
//------------------------------------------------------------------
StatusType TargetPort::QSend(QHandle qId, const void* buf)
{
    return xQueueSend(qId, buf, 0);
}

//------------------------------------------------------------------
// QRecv
//------------------------------------------------------------------
StatusType TargetPort::QRecv(QHandle qId, void* buf, uint32_t timeout)
{
    return xQueueReceive(qId, buf, timeout);
}
