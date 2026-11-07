/// @file StdOutPort.c
/// @brief StdOutPort implementation for ESP32 / ESP-IDF, supporting both Picolibc and Newlib.
/// @details ESP-IDF v6.0 and later use Picolibc by default (CONFIG_LIBC_PICOLIBC). Earlier versions, or
///          projects that select CONFIG_LIBC_NEWLIB, use Newlib. In both cases the C library's stdout write
///          function is saved and replaced with one that writes into the printf buffer. The saved function
///          is then used to output the buffered data when the PrintfTask runs.
///
///          This is the only file that needs to change when porting to another target. For example, on an
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

#include "sdkconfig.h"

#include <stdio.h>

#if CONFIG_LIBC_PICOLIBC

//==================================================================
// Picolibc
//==================================================================
#include <stdint.h>
#include <stdio-bufio.h>

/// @brief Original stdout write function, used to output the buffered data from the PrintfTask
static ssize_t (*realStdOutWrite)(int, const void*, size_t) = NULL;

/// @brief File descriptor of stdout, passed to realStdOutWrite
static int stdOutFd = 1;

//------------------------------------------------------------------
// WriteReroute
//------------------------------------------------------------------
/// @brief Replacement stdout write function, called by Picolibc for printf output
/// @details Picolibc holds the stdout lock while calling this, so writes from different tasks are already
///          serialized and no extra locking is needed here.
/// @param[in] fd - file descriptor (unused)
/// @param[in] data - data to be written
/// @param[in] len - length of data to be written, in bytes
/// @return number of bytes written to the printf buffer
static ssize_t WriteReroute(int fd, const void* data, size_t len)
{
    (void)fd;
    return PrintBuffer_Write((const char*)data, (int)len);
}

//------------------------------------------------------------------
// StdOutPort_Redirect
//------------------------------------------------------------------
void StdOutPort_Redirect(void)
{
    // ESP-IDF sets stdout up as a Picolibc buffered (bufio) file using a file descriptor
    struct __file_bufio* const bufio = (struct __file_bufio*)stdout;

    stdOutFd = (int)(intptr_t)bufio->ptr;
    realStdOutWrite = bufio->write_int;
    bufio->write_int = WriteReroute;
}

//------------------------------------------------------------------
// StdOutPort_Write
//------------------------------------------------------------------
int StdOutPort_Write(const char* data, int len)
{
    return (int)realStdOutWrite(stdOutFd, data, (size_t)len);
}

#else

//==================================================================
// Newlib
//==================================================================

/// @brief Original stdout write function, used to output the buffered data from the PrintfTask
static int (*realStdOutWrite)(struct _reent*, void*, const char*, int) = NULL;

//------------------------------------------------------------------
// WriteReroute
//------------------------------------------------------------------
/// @brief Replacement stdout write function, called by Newlib for printf output
/// @details ESP-IDF's Newlib already serializes access to stdout between tasks, so no extra locking is
///          needed here.
/// @param[in] r - Newlib reentrancy structure (unused)
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

#endif
