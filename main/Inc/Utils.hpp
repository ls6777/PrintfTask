#pragma once

#include "driver/gpio.h"

#include "TargetPort.hpp"

#define DELAY_MS(x) DELAY(MS_TO_TICKS(x))

// GPIOs used to measure printf timing with a logic analyzer/scope
#define GPIO_OUTPUT_IO_0    GPIO_NUM_18
#define GPIO_OUTPUT_IO_1    GPIO_NUM_19
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
