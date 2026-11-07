/// @file main.cpp
/// @brief Example application: creates the PrintfTask and the example tasks.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "PrintfTask.hpp"
#include "Task1.hpp"
#include "Task2.hpp"
#include "Utils.hpp"
#include "TargetPort.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
    void app_main(void); ///< ESP-IDF application entry point (C linkage)
#ifdef __cplusplus
}
#endif

constexpr uint32_t PRINTF_TASK_STACK_SIZE = 2048;                   ///< PrintfTask stack size, in bytes
constexpr UBaseType_t PRINTF_TASK_PRIORITY = 2;                     ///< PrintfTask priority (low)

constexpr uint32_t TASK_1_STACK_SIZE = 2048;                        ///< Task1 stack size, in bytes
constexpr UBaseType_t TASK_1_PRIORITY = configMAX_PRIORITIES - 1;   ///< Task1 priority (highest)

constexpr uint32_t TASK_2_STACK_SIZE = 2048;                        ///< Task2 stack size, in bytes
constexpr UBaseType_t TASK_2_PRIORITY = configMAX_PRIORITIES - 2;   ///< Task2 priority

//------------------------------------------------------------------
// StartPrintfTask
//------------------------------------------------------------------
/// @brief Entry point for the PrintfTask task. Never returns.
/// @param[in] argument - task argument (unused)
void StartPrintfTask(void *argument)
{
    PrintfTask task;
    task.Initialize();
    task.Run();
}

//------------------------------------------------------------------
// StartTask1
//------------------------------------------------------------------
/// @brief Entry point for the Task1 task. Never returns.
/// @param[in] argument - task argument (unused)
void StartTask1(void *argument)
{
    Task1 task;
    task.Initialize();
    task.Run();
}

//------------------------------------------------------------------
// StartTask2
//------------------------------------------------------------------
/// @brief Entry point for the Task2 task. Never returns.
/// @param[in] argument - task argument (unused)
void StartTask2(void *argument)
{
    Task2 task;
    task.Initialize();
    task.Run();
}

//------------------------------------------------------------------
// SetupGpio
//------------------------------------------------------------------
/// @brief Configure the GPIO outputs used to measure printf timing
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
/// @brief Application entry point: sets up the GPIO and creates all tasks
void app_main(void)
{
    // Setup GPIO
    SetupGpio();

    // Printf task
    xTaskCreate(&StartPrintfTask, "PrintfTask", PRINTF_TASK_STACK_SIZE, nullptr, PRINTF_TASK_PRIORITY, nullptr);

    // Task 1
    xTaskCreate(&StartTask1, "Task1", TASK_1_STACK_SIZE, nullptr, TASK_1_PRIORITY, nullptr);

    // Task 2
    xTaskCreate(&StartTask2, "Task2", TASK_2_STACK_SIZE, nullptr, TASK_2_PRIORITY, nullptr);
}

