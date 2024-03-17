#pragma once
#include "driver/gpio.h"

#define DELAY_MS(x) vTaskDelay(pdMS_TO_TICKS(x))
#define KERNEL_TICKS_IN_MS() pdTICKS_TO_MS(xTaskGetTickCount())

static constexpr uint32_t WORD_SIZE = sizeof(uint32_t);
static constexpr uint32_t DOUBLE_WORD_SIZE = sizeof(uint64_t);

static constexpr uint32_t MS_PER_SEC = 1000;
static constexpr uint32_t SECS_PER_MIN = 60;
static constexpr uint32_t MINS_PER_HOUR = 60;
static constexpr uint32_t HOURS_PER_DAY = 24;

static constexpr uint32_t BYTES_PER_KBYTE = 1024;
static constexpr uint32_t KBYTES_PER_MBYTE = 1024;
static constexpr uint32_t BYTES_PER_MBYTE = BYTES_PER_KBYTE * KBYTES_PER_MBYTE;

#define GPIO_OUTPUT_IO_0    GPIO_NUM_18
#define GPIO_OUTPUT_IO_1    GPIO_NUM_19
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))

