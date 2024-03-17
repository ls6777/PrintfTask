
#include "TargetPort.hpp"

//------------------------------------------------------------------
// CreateQ
//------------------------------------------------------------------
QHandle CreateQ(uint32_t qLength, uint32_t itemSize)
{
    return xQueueCreate(qLength, itemSize);
}

//------------------------------------------------------------------
// QSend
//------------------------------------------------------------------
StatusType QSend(QHandle qId, void* const buf)
{
    return xQueueSend(qId, buf, 0);
}

//------------------------------------------------------------------
// QRecv
//------------------------------------------------------------------
StatusType QRecv(QHandle qId, void* const buf, uint32_t timeout)
{
    return xQueueReceive(qId, buf, timeout);
}
