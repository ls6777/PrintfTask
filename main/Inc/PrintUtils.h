#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Setup reroute of stdout
/// @details Retargets the C library printf function to a circular buffer.
void InitStdOut();

/// @brief get the current high water mark for the buffer
/// @return most space taken in the printf buffer, since app start, in bytes
int GetHighWaterMark();

/// @brief Transmit any data available in the buffer
int TransmitData();

#ifdef __cplusplus
}
#endif

#endif /* PRINT_UTILS_H_ */
