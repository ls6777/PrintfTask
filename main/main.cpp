#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "PrintfTask.hpp"
#include "Task1.hpp"
#include "Task2.hpp"
#include "Utils.hpp"

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
    PrintfTask task;
    task.Initialize();
    task.Run();
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
    Task2 task;
    task.Initialize();
    task.Run();
}

void SetupGpio()
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

//------------------------------------------------------------------
// Main Task
//------------------------------------------------------------------
void app_main(void)
{
    TaskHandle_t taskHandle = nullptr;

    // Setup GPIO
    SetupGpio();

    // Printf task
    xTaskCreate(&StartPrintfTask, "PrintfTask", PRINTF_TASK_STACK_SIZE, NULL, PRINTF_TASK_PRIORITY, &taskHandle);

    // Task 1
    xTaskCreate(&StartTask1, "Task1", TASK_1_STACK_SIZE, NULL, TASK_1_PRIORITY, &taskHandle);

    // task 2
    xTaskCreate(&StartTask2, "Task2", TASK_2_STACK_SIZE, NULL, TASK_2_PRIORITY, &taskHandle);
}

