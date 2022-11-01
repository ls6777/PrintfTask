#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "PrintfTask.hpp"
#include "Task1.hpp"
//#include "Task2.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
    void app_main(void);
#ifdef __cplusplus
}
#endif

constexpr uint32_t PRINTF_TASK_STACK_SIZE = 2048;
constexpr UBaseType_t PRINTF_TASK_PRIORITY = 2;

constexpr uint32_t TASK_1_STACK_SIZE = 2048;
constexpr UBaseType_t TASK_1_PRIORITY = configMAX_PRIORITIES - 1;

constexpr uint32_t TASK_2_STACK_SIZE = 2048;
constexpr UBaseType_t TASK_2_PRIORITY = configMAX_PRIORITIES - 2;

//------------------------------------------------------------------
// StartPrintfTask
//------------------------------------------------------------------
void StartPrintfTask(void *argument)
{
//    PrintfTask task;
//    task.Initialize();
//    task.Run();
}

//------------------------------------------------------------------
// StartTask1
//------------------------------------------------------------------
void StartTask1(void *argument)
{
    Task1 task;
    task.Initialize();
    task.Run();
}

//------------------------------------------------------------------
// StartTask2
//------------------------------------------------------------------
void StartTask2(void *argument)
{
//    Task2 task;
//    task.Initialize();
//    task.Run();
}

//------------------------------------------------------------------
// Main Task
//------------------------------------------------------------------
void app_main(void)
{
    TaskHandle_t taskHandle = nullptr;

    // Printf task
    xTaskCreate(&StartPrintfTask, "PrintfTask", PRINTF_TASK_STACK_SIZE, NULL, PRINTF_TASK_PRIORITY, &taskHandle);

    // Task 1
    xTaskCreate(&StartTask1, "Task1", TASK_1_STACK_SIZE, NULL, TASK_1_PRIORITY, &taskHandle);

    // task 2
    xTaskCreate(&StartTask2, "Task2", TASK_2_STACK_SIZE, NULL, TASK_2_PRIORITY, &taskHandle);
}

