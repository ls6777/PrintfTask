#ifndef PRINTFBUFFER_H_
#define PRINTFBUFFER_H_

/*!
 * @class PrintfBuffer
 * @brief This class implements a circular buffer to store printf strings.
 */
class PrintfBuffer {
public:
	PrintfBuffer();

	/*!
		 * @fn int push(const char*, const int)
	 * @brief Stores passed in character array in the buffer.
	 *
	 * @param data Pointer to the character array
	 * @param length length of character array
	 * @return Number of characters added to the buffer. If the buffer does not
	 *         have enough space left to store all the data, it stores what it can
	 *         and return value will reflect the number of bytes stored.
	 */
	int push(const char* data, const int length);

	/*!
		 * @fn const char* pop(const int, int&)
	 * @brief Pops the next chunk of available data. If the distance between the head and the
	 *        buffer end is less than the requested data size, length is set to the distance
	 *        between the head and the buffer and end the head is set to the beginning of the buffer.
	 *        The caller can check to see if there's more data remaining and make another call to
	 *        getNextChunk to get the remaining data.
	 *
	 * @param chunkSize The amount of data requested.
	 * @param length The amount of data returned. This can be less than chunkSize if
	 *        There's not enough data in the buffer or the distance between the head and
	 *        the buffer end is less than the requested chunk size.
	 * @return
	 */
	const char* pop(const int chunkSize, int& length);

	/*!
		 * @fn int getSize()
	 * @brief Returns the number of characters left in the buffer
	 *
	 * @return Number of characters in the buffer.
	 */
	int getSize();

	/*!
		 * @fn int getHighWatermark()
	 * @brief Returns the highest ever number of bytes in the buffer.
	 *
	 * @return High watermark value.
	 */
	int getHighWatermark();

	virtual ~PrintfBuffer() {}

private:
	static const int BUFFER_SIZE = 4 * 1024;
	char mBuffer[BUFFER_SIZE];
	char* mBufferEnd = mBuffer + BUFFER_SIZE - 1;

	char* mBufferHead;
	char* mBufferTail;
	int mHighWatermark;
	int mRemainingByteCount;
};

#endif /* PRINTFBUFFER_H_ */
