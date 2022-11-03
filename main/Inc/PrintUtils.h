#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Write the provide data out the provide put char function
/// @param fd - unused
/// @param ptr - buffer where data to be transmitted is located
/// @param len - len of data to be transmitted
/// @return len of data to be transmitted
int _write(int fd, char *ptr, int len);

/// @brief calculates the length currently used in the buffer
/// @param cHead - head pointer in buffer
/// @param cTail - tail pointer in buffer
/// @return calculated length occupied in the buffer
int CalculateLength(char* cHead, char* cTail);

/// @brief Transmit any data available in the buffer
void TransmitData();

#ifdef __cplusplus
}
#endif

#endif /* PRINT_UTILS_H_ */
