#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

/// @file PrintUtils.h
/// @brief Target independent stdout API used by the PrintfTask.
/// @details Ties the circular buffer (PrintBuffer.h) to the target's stdout (StdOutPort.h).

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Set up the reroute of stdout
/// @details Redirects stdout, and therefore printf, into the circular buffer (see PrintBuffer.h).
///          Must be called once before any output is expected to be buffered.
void InitStdOut(void);

/// @brief Get the high water mark of the printf buffer
/// @return most space taken in the printf buffer since application start, in bytes
int GetHighWaterMark(void);

/// @brief Transmit all data available in the buffer to the real stdout device
/// @return amount of data transmitted, in bytes, or a negative value on error
int TransmitData(void);

#ifdef __cplusplus
}
#endif

#endif /* PRINT_UTILS_H_ */
