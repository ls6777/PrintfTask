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
 *
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

	static std::unique_ptr<PrintfBuffer> mBuffer;

};

#endif /* PRINTFTASK_H_ */
