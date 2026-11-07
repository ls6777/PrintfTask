#include "Task1.hpp"
#include "Utils.hpp"

#include <cstdio>

QHandle Task1::msgQHandle;

//------------------------------------------------------------------
// SendMessage
//------------------------------------------------------------------
void Task1::SendMessage(Message::MsgId id)
{
    Message msg(id);
    const StatusType status = QSend(msgQHandle, &msg);
    ASSERT(PASS == status);
    (void)status;
}

//------------------------------------------------------------------
// Process
//------------------------------------------------------------------
void Task1::Process()
{
    SendMessage(Message::PROCESS);
}

//------------------------------------------------------------------
// Shutdown
//------------------------------------------------------------------
void Task1::Shutdown()
{
    SendMessage(Message::SHUTDOWN);
}

//------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------
void Task1::Initialize()
{
    msgQHandle = CreateQ(10, sizeof(Message));
    SendMessage(Message::INITIALIZE);
}

//------------------------------------------------------------------
// Run
//------------------------------------------------------------------
void Task1::Run()
{
    StatusType status = FALSE;
    Message msg;

    while (true)
    {
        // Wait for new message to process indefinitely
        status = QRecv(msgQHandle, &msg, MAX_DELAY);

        // Check status of message Q result
        if (TRUE == status)
        {
            // process message
            switch (msg.msgId)
            {
                case Message::INITIALIZE:
                    HandleInitialize();
                    break;

                case Message::PROCESS:
                    HandleProcess();
                    break;

                case Message::SHUTDOWN:
                    HandleShutdown();
                    break;

                default:
                    printf("task1 - unknown msgId: %d\r\n", msg.msgId);
                    break;
            }
        }
    }
}

//------------------------------------------------------------------
// HandleInitialize
//------------------------------------------------------------------
void Task1::HandleInitialize()
{
    printf("Task1 Initialized\r\n");
    Process();
}

//------------------------------------------------------------------
// HandleProcess
//------------------------------------------------------------------
void Task1::HandleProcess()
{
    static uint32_t count = 0;

    count++;

//    gpio_set_level(GPIO_OUTPUT_IO_0, 1); // used for debugging or timing
    printf("Task1 stuff: %lu\r\n", (unsigned long)count);
//    gpio_set_level(GPIO_OUTPUT_IO_0, 0); // used for debugging or timing

    DELAY_MS(10);
    Process();
}

//------------------------------------------------------------------
// HandleShutdown
//------------------------------------------------------------------
void Task1::HandleShutdown()
{
    printf("Task1 Shutdown\r\n");
    // wait forever
    DELAY(MAX_DELAY);
}
