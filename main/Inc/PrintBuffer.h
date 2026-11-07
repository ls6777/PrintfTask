#ifndef PRINT_BUFFER_H_
#define PRINT_BUFFER_H_

/// @file PrintBuffer.h
/// @brief Target independent circular buffer used to hold printf output until it can be transmitted.
/// @details The buffer is a single producer / single consumer queue. The producer is whatever is writing
///          to stdout (printf) and the consumer is the PrintfTask. No OS or C library specifics are used
///          here so the logic can be unit tested on a host machine.

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Total size of the circular buffer in bytes.
/// @note One byte is always kept free to distinguish a full buffer from an empty one, so the usable
///       capacity is PRINTF_BUFFER_SIZE - 1.
#ifndef PRINTF_BUFFER_SIZE
#define PRINTF_BUFFER_SIZE 15000
#endif

/// @brief Function used to output data from the buffer (e.g. the original stdout write function)
/// @param[in] data - data to be output
/// @param[in] len - length of data to output, in bytes (always > 0)
/// @return number of bytes actually output, or a negative value on error
typedef int (*PrintBufferOutputFn)(const char* data, int len);

/// @brief Copy data into the buffer
/// @details If there isn't enough space for all of the data, only what fits is copied.
/// @param[in] data - data to be written
/// @param[in] len - length of data to be written, in bytes
/// @return number of bytes copied into the buffer
int PrintBuffer_Write(const char* data, int len);

/// @brief Output all data currently in the buffer
/// @details Output may take two calls to the output function when the data wraps around the end of the
///          buffer. Stops early if the output function outputs less than requested.
/// @param[in] output - function used to output the data
/// @return number of bytes output, or the negative error value returned by the output function
int PrintBuffer_Transmit(PrintBufferOutputFn output);

/// @brief Get the number of bytes currently held in the buffer
/// @return bytes in use
int PrintBuffer_GetOccupiedSize(void);

/// @brief Get the most bytes ever held in the buffer since start (or last reset)
/// @details Useful for choosing an appropriate PRINTF_BUFFER_SIZE.
/// @return high water mark, in bytes
int PrintBuffer_GetHighWaterMark(void);

/// @brief Discard all buffered data and clear the high water mark
void PrintBuffer_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /* PRINT_BUFFER_H_ */
