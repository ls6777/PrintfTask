/// @file Task2.cpp
/// @brief Example Task2 implementation (see Task2.hpp).

#include "Task2.hpp"
#include "Utils.hpp"
#include "PrintUtils.hpp"

#include <cstdio>

static char taskListBuf[500];   ///< Output of vTaskList()
static char taskStatsBuf[500];  ///< Output of vTaskGetRunTimeStats()

QHandle Task2::msgQHandle;

//------------------------------------------------------------------
// SendMessage
//------------------------------------------------------------------
void Task2::SendMessage(Message::MsgId id)
{
    Message msg(id);
    const StatusType status = QSend(msgQHandle, &msg);
    ASSERT(PASS == status);
    (void)status;
}

//------------------------------------------------------------------
// Process
//------------------------------------------------------------------
void Task2::Process()
{
    SendMessage(Message::PROCESS);
}

//------------------------------------------------------------------
// Shutdown
//------------------------------------------------------------------
void Task2::Shutdown()
{
    SendMessage(Message::SHUTDOWN);
}

//------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------
void Task2::Initialize()
{
    msgQHandle = CreateQ(10, sizeof(Message));
    SendMessage(Message::INITIALIZE);
}

//------------------------------------------------------------------
// Run
//------------------------------------------------------------------
void Task2::Run()
{
    constexpr uint32_t MSG_Q_TIMEOUT = 5000; // ms to wait for a message before printing the stats
    StatusType status = FALSE;
    Message msg;

    while (true)
    {
        // Wait for new message to process up to MSG_Q_TIMEOUT
        status = QRecv(msgQHandle, &msg, MS_TO_TICKS(MSG_Q_TIMEOUT));

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
                    printf("task2 - unknown msgId: %d\r\n", msg.msgId);
                    break;
            }
        }
        else
        {
            Process();
        }
    }
}

//------------------------------------------------------------------
// HandleInitialize
//------------------------------------------------------------------
void Task2::HandleInitialize()
{
    printf("Task2 Initialized\r\n");
}

//------------------------------------------------------------------
// HandleProcess
//------------------------------------------------------------------
void Task2::HandleProcess()
{
    vTaskList(taskListBuf);
//    gpio_set_level(GPIO_OUTPUT_IO_0, 1); // used for debugging or timing
    printf("\r\nTASK INFO**********************************************\r\n");
    printf("Name          State  Priority   Stack   Num    Core\r\n");
    printf("*******************************************************\r\n");
    printf("%s\r\n", taskListBuf);
    printf("\r\n");
//    gpio_set_level(GPIO_OUTPUT_IO_0, 0); // used for debugging or timing

    vTaskGetRunTimeStats(taskStatsBuf);
//    gpio_set_level(GPIO_OUTPUT_IO_1, 1); // used for debugging or timing
    printf("TASK STATS INFO****************************************\r\n");
    printf("Name             Abs Time       %% Time\r\n");
    printf("*******************************************\r\n");
    printf("%s\r\n", taskStatsBuf);
    printf("\r\n");

    printf("High Water Mark: %d\r\n", PrintUtils::GetHighWaterMark());
    printf("\r\n");
//    gpio_set_level(GPIO_OUTPUT_IO_1, 0); // used for debugging or timing
}

//------------------------------------------------------------------
// HandleShutdown
//------------------------------------------------------------------
void Task2::HandleShutdown()
{
    printf("Task2 Shutdown\r\n");
    // wait forever
    DELAY(MAX_DELAY);
}
