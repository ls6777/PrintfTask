#include <string.h>
#include <algorithm>
#include "cmsis_os2.h"
#include "PrintfTask.h"
#include "PrintfBuffer.h"

STDOUT_PRINT_FUC PrintfTask::mPrintFuc = nullptr;
std::unique_ptr<PrintfBuffer> PrintfTask::mBuffer = nullptr;

//------------------------------------------------------------------
// PrintfTask
//------------------------------------------------------------------
PrintfTask::PrintfTask() {

	mBuffer.reset(new PrintfBuffer);
	assert(mBuffer != nullptr);

	mPrintFuc = _GLOBAL_REENT->_stdout->_write;
	_GLOBAL_REENT->_stdout->_write = writeReroute;
}

//------------------------------------------------------------------
// writeReroute
//------------------------------------------------------------------
int PrintfTask::writeReroute(struct _reent *r, void *fd, const char *data, int len) {

	return mBuffer->push(data, len);
}

//------------------------------------------------------------------
// procedure
//------------------------------------------------------------------
void PrintfTask::run() {
	while (true) {
		int sendDataLength;
		auto nextDataChunk = mBuffer->pop(1024, sendDataLength);
		int dataTransfered = mPrintFuc(_GLOBAL_REENT, _GLOBAL_REENT->_stdout, nextDataChunk, sendDataLength);

		assert(dataTransfered == sendDataLength);
		osDelay(100);
	}
}
