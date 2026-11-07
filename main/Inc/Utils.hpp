#pragma once

/// @file Utils.hpp
/// @brief Helpers and pin definitions used by the example application.

#include "driver/gpio.h"

#include "TargetPort.hpp"

/// @brief Block the calling task for a number of milliseconds
#define DELAY_MS(x) DELAY(MS_TO_TICKS(x))

#define GPIO_OUTPUT_IO_0    GPIO_NUM_18     ///< Output toggled to measure printf timing with a logic analyzer/scope
#define GPIO_OUTPUT_IO_1    GPIO_NUM_19     ///< Output toggled to measure printf timing with a logic analyzer/scope

/// @brief Bit mask of all GPIO outputs, for gpio_config()
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
