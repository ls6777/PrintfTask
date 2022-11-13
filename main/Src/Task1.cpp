
#include "Task1.hpp"
#include "Message.hpp"
#include "Utils.hpp"
#include "driver/gpio.h"

QHandle Task1::msgQHandle;

//------------------------------------------------------------------
// Process
//------------------------------------------------------------------
void Task1::Process()
{
    Message msg(Message::PROCESS);
    ASSERT(PASS == QSend(msgQHandle, &msg));
}

//------------------------------------------------------------------
// Shutdown
//------------------------------------------------------------------
void Task1::Shutdown()
{
    Message msg(Message::SHUTDOWN);
    ASSERT(PASS == QSend(msgQHandle, &msg));
}

//------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------
void Task1::Initialize()
{
    msgQHandle = CreateQ(10, sizeof(Message));
    Message msg(Message::INITIALIZE);
    ASSERT(PASS == QSend(msgQHandle, &msg));
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

    gpio_set_level(GPIO_OUTPUT_IO_0, 1);
    printf("Task1 stuff: %u\r\n", count);
    gpio_set_level(GPIO_OUTPUT_IO_0, 0);

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
