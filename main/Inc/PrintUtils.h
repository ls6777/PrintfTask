#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void InitStdOut();

/// @brief Write the provide data out the provide put char function
/// @param fd - unused
/// @param ptr - buffer where data to be transmitted is located
/// @param len - len of data to be transmitted
/// @return len of data to be transmitted
int my_write(struct _reent* r, void *fd, const char *c, int len);

/// @brief calculates the length currently used in the buffer
/// @param cHead - head pointer in buffer
/// @param cTail - tail pointer in buffer
/// @return calculated length occupied in the buffer
//int CalculateLength();

int GetHighWaterMark();

/// @brief Transmit any data available in the buffer
int TransmitData();

#ifdef __cplusplus
}
#endif

#endif /* PRINT_UTILS_H_ */
