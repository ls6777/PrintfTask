
#include "Task2.hpp"
#include "Message.hpp"
#include "Utils.hpp"

// buffers for task stats
static char taskListBuf[500];
static char taskStatsBuf[500];

QHandle Task2::msgQHandle;

//------------------------------------------------------------------
// Process
//------------------------------------------------------------------
void Task2::Process()
{
    Message msg(Message::PROCESS);
    ASSERT(PASS == QSend(msgQHandle, &msg));
}

//------------------------------------------------------------------
// Shutdown
//------------------------------------------------------------------
void Task2::Shutdown()
{
    Message msg(Message::SHUTDOWN);
    ASSERT(PASS == QSend(msgQHandle, &msg));
}

//------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------
void Task2::Initialize()
{
    msgQHandle = CreateQ(10, sizeof(Message));
    Message msg(Message::INITIALIZE);
    ASSERT(PASS == QSend(msgQHandle, &msg));
}

//------------------------------------------------------------------
// Run
//------------------------------------------------------------------
void Task2::Run()
{
    constexpr uint32_t MSG_Q_TIMEOUT = 5000; // ms
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
    printf("\r\nTASK INFO**********************************************\r\n");
    printf("Name          State  Priority   Stack   Num    Core\r\n");
    printf("*******************************************************\r\n");
    printf("%s\r\n", taskListBuf);
    printf("\r\n");

    vTaskGetRunTimeStats(taskStatsBuf);
    printf("TASK STATS INFO****************************************\r\n");
    printf("Name             Abs Time       %% Time\r\n");
    printf("*******************************************\r\n");
    printf("%s\r\n", taskStatsBuf);
    printf("\r\n");
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
