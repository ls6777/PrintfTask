/// @file StdOutPort.c
/// @brief StdOutPort implementation for ESP32 / ESP-IDF (newlib).
/// @details This is the only file that needs to change when porting to another target. For example, on an
///          STM32 using STM32CubeIDE (newlib-nano), StdOutPort_Redirect can be empty and the _write syscall
///          overridden instead:
/// @code
/// int _write(int fd, char* ptr, int len)
/// {
///     return PrintBuffer_Write(ptr, len);
/// }
///
/// int StdOutPort_Write(const char* data, int len)
/// {
///     return (HAL_UART_Transmit(&huart2, (uint8_t*)data, len, HAL_MAX_DELAY) == HAL_OK) ? len : -1;
/// }
/// @endcode

#include "StdOutPort.h"
#include "PrintBuffer.h"

#include <stdio.h>

/// @brief Original stdout write function, used to output the buffered data from the PrintfTask
static int (*realStdOutWrite)(struct _reent*, void*, const char*, int) = NULL;

//------------------------------------------------------------------
// WriteReroute
//------------------------------------------------------------------
/// @brief Replacement stdout write function, called by newlib for printf output
/// @details ESP-IDF's newlib already serializes access to stdout between tasks, so no extra locking is
///          needed here.
/// @param[in] r - newlib reentrancy structure (unused)
/// @param[in] cookie - stream cookie (unused)
/// @param[in] data - data to be written
/// @param[in] len - length of data to be written, in bytes
/// @return number of bytes written to the printf buffer
static int WriteReroute(struct _reent* r, void* cookie, const char* data, int len)
{
    (void)r;
    (void)cookie;
    return PrintBuffer_Write(data, len);
}

//------------------------------------------------------------------
// StdOutPort_Redirect
//------------------------------------------------------------------
void StdOutPort_Redirect(void)
{
    realStdOutWrite = _GLOBAL_REENT->_stdout->_write;
    _GLOBAL_REENT->_stdout->_write = WriteReroute;
}

//------------------------------------------------------------------
// StdOutPort_Write
//------------------------------------------------------------------
int StdOutPort_Write(const char* data, int len)
{
    return realStdOutWrite(_GLOBAL_REENT, _GLOBAL_REENT->_stdout, data, len);
}
