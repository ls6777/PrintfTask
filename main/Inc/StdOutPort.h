#ifndef STD_OUT_PORT_H_
#define STD_OUT_PORT_H_

/// @file StdOutPort.h
/// @brief Target specific stdout hooks used by PrintUtils.
/// @details These are the only functions that need to be implemented when porting PrintfTask to a new
///          target / C library. See StdOutPort.c for the ESP32 (newlib) implementation.

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Redirect stdout so data written by printf goes into the printf buffer
/// @details The implementation must:
///          - save whatever is needed for StdOutPort_Write to output to the real stdout device
///          - route all stdout writes to PrintBuffer_Write (see PrintBuffer.h)
///          If the target/C library doesn't serialize stdout access between tasks, the redirected write
///          must be protected with a lock/mutex.
void StdOutPort_Redirect(void);

/// @brief Write data to the real stdout device (e.g. UART)
/// @param[in] data - data to output
/// @param[in] len - length of data to output, in bytes
/// @return number of bytes output, or a negative value on error
int StdOutPort_Write(const char* data, int len);

#ifdef __cplusplus
}
#endif

#endif /* STD_OUT_PORT_H_ */
