#ifndef PRINTFTASK_H_
#define PRINTFTASK_H_

#include <stdio.h>
#include <memory>

typedef int (*STDOUT_PRINT_FUC)(struct _reent *, void *, const char *, int);

class PrintfBuffer;

/*!
 * @class PrintfTask
 * @brief A thread that reroutes printf data and store it in a buffer and periodically
 * 		  outputs it through a given medium (e.g., UART).
 * @details Upon creation, this thread reroutes stdout to writeReroute. Afterwards, whenever calls
 * 			to printf are made, the data is stored in a buffer instead if immediately being sent out.
 * 			PrintfTask will retrieve a chunk of data from the buffer when it gets a chance to run and
 * 			transmits the data.
 *			The benefit of this approach is that the thread that calls the printf does get blocked until
 *			the data is transmitted and as a result calls to printf become much cheaper.
 *
 * @note	This thread should have low priority since it uses the serial port to send out the printf data.
 * 			As an improvement, the serial port can be configure to use interrupts or the DMA to make sending
 * 			data on the serial port more efficient.
 */
class PrintfTask{
public:
	PrintfTask();
	void run();

	virtual ~PrintfTask() {}

private:
	static int writeReroute(struct _reent* r, void* fd, const char* data, int len);

	/// function pointer that's used to flush the buffer when writing out from task
	static STDOUT_PRINT_FUC mPrintFuc;

	/// Pointer to the buffer used to store the printf data when printf is called.
	static std::unique_ptr<PrintfBuffer> mBuffer;

	/// The chunk of data in bytes that the thread will attempt to send out when it runs.
	static const uint32_t DATA_CHUNK_SIZE = 1024;

	/// The number of ticks that the task will sleep for after each transmission.
	static const uint32_t TASK_DELAY_TICKS = 100;
};

#endif /* PRINTFTASK_H_ */
