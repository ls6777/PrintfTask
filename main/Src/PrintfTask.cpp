#include "PrintfTask.hpp"
#include "Message.hpp"
#include "Utils.hpp"

QHandle DefaultTask::msgQHandle;

//------------------------------------------------------------------
// Shutdown
//------------------------------------------------------------------
void PrintfTask::Shutdown()
{
    Message msg(Message::SHUTDOWN);
    ASSERT(PASS == QSend(msgQHandle, &msg));
}

//------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------
void PrintfTask::Initialize()
{
    msgQHandle = CreateQ(10, sizeof(Message));
    Message msg(Message::INITIALIZE);
    ASSERT(PASS == QSend(msgQHandle, &msg));
}

//------------------------------------------------------------------
// Run
//------------------------------------------------------------------
void PrintfTask::Run()
{
    constexpr uint32_t MSG_Q_TIMEOUT = 10; // ms
    StatusType status = FALSE;
    Message msg;

    while (true)
    {
        // wait for message up to MSG_Q_TIMEOUT
        status = QRecv(msgQHandle, &msg, MS_TO_TICKS(MSG_Q_TIMEOUT));

        if (TRUE == status)
        {
            switch (msg.msgId)
            {
                case Message::INITIALIZE:
                    HandleInitialize();
                    break;

                case Message::SHUTDOWN:
                    HandleShutdown();
                    break;

                default:
                    printf("PrintfTask - unknown msgId: %d\r\n", msg.msgId);
                    break;
            }
        }

        // Print out any data in the buffer

    }
}

//---------------------------------------------------
// HandleInitialize
//---------------------------------------------------
void PrintfTask::HandleInitialize()
{
    printf("Printf Task Initialized\r\n");
}

//------------------------------------------------------------------
// HandleShutdown
//------------------------------------------------------------------
void PrintfTask::HandleShutdown()
{
    printf("Printf Task Shutdown\r\n");
    // wait forever
    DELAY(MAX_DELAY);
}
