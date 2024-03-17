#include <algorithm>
#include <string.h>

#include "PrintfBuffer.h"

//------------------------------------------------------------------
// PrintfBuffer
//------------------------------------------------------------------
PrintfBuffer::PrintfBuffer() :
	mBufferHead(mBuffer),
	mBufferTail(mBuffer),
	mHighWatermark(0),
	mRemainingByteCount(0) {
}

//------------------------------------------------------------------
// push
//------------------------------------------------------------------
int PrintfBuffer::push(const char *data, const int length) {
	if(length <= 0)
		return 0;

	// Trim the data if there's not enough space left.
	auto dataLength = std::min(length, BUFFER_SIZE - mRemainingByteCount);

	// The distance between the tail and the buffer end is enough to fit the data.
	if (mBufferTail + dataLength < mBufferEnd)
	{
		// for loop implementation is simpler, but this memcpy implementation is faster to execute
		memcpy(mBufferTail, data, dataLength);
		mBufferTail += dataLength;
	}
	else
	{
		auto byteUntilBufferEnd =  mBufferEnd - mBufferTail;
		memcpy(mBufferTail, data, byteUntilBufferEnd);

		// Copy the remainder to the beginning. First set the tail to the beginning of the buffer.
		// Then copy the remaining data bytes starting from the beginning of the array.
		auto remainingDataBytes = dataLength - byteUntilBufferEnd;
		mBufferTail = mBuffer;
		memcpy(mBufferTail, data + byteUntilBufferEnd, remainingDataBytes);
		mBufferTail += remainingDataBytes;
	}

	mRemainingByteCount += dataLength;

	// update high water mark of buffer if needed. This can help developer determine appropriate size
	mHighWatermark = std::max(mHighWatermark, mRemainingByteCount);

	return dataLength;
}

//------------------------------------------------------------------
// getNextChunk
//------------------------------------------------------------------
const char* PrintfBuffer::pop(const int chunkSize, int &length) {
	auto byteUntilBufferEnd = mBufferEnd - mBufferHead;

	if (chunkSize > mRemainingByteCount) {
		length = mRemainingByteCount;
	}

	// We don't want head + mByteCount to go beyond the buffer end.
	length = std::min(byteUntilBufferEnd, length);

	// Store the head in the temp folder to return.
	auto tempHead = mBufferHead;

	// Move forward the head and update the byte count
	mBufferHead += length;
	if (mBufferHead == mBufferEnd) {
		mBufferHead = mBuffer;
	}

	mRemainingByteCount -= length;

	return tempHead;
}

//------------------------------------------------------------------
// getRemainingByteCount
//------------------------------------------------------------------
int PrintfBuffer::getSize() {
	return mRemainingByteCount;
}

//------------------------------------------------------------------
// getHighWatermark
//------------------------------------------------------------------
int PrintfBuffer::getHighWatermark() {
	return mHighWatermark;
}
