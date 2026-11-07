#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Setup reroute of stdout
/// @details Retargets the C library printf function to a circular buffer (see PrintBuffer.h).
void InitStdOut(void);

/// @brief get the current high water mark for the buffer
/// @return most space taken in the printf buffer, since application start, in bytes
int GetHighWaterMark(void);

/// @brief Transmit any data available in the buffer
/// @return Amount of data transmitted, in bytes, or a negative value on error
int TransmitData(void);

#ifdef __cplusplus
}
#endif

#endif /* PRINT_UTILS_H_ */
