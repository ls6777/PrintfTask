#include "unity.h"
#include "PrintBuffer.h"

#include <string.h>

// The test build sets PRINTF_BUFFER_SIZE to 16, so usable capacity is 15 bytes
#define CAPACITY (PRINTF_BUFFER_SIZE - 1)

//------------------------------------------------------------------
// Fake output function
//------------------------------------------------------------------
static char output[1024];
static int outputLen;
static int outputCalls;
static int outputLimit;   // max bytes accepted per call, < 0 for unlimited
static int outputResult;  // if non-zero, returned instead of writing (e.g. an error)

static int FakeOutput(const char* data, int len)
{
    outputCalls++;
    TEST_ASSERT_GREATER_THAN_INT(0, len);

    if (outputResult != 0)
    {
        return outputResult;
    }

    if ((outputLimit >= 0) && (len > outputLimit))
    {
        len = outputLimit;
    }
    memcpy(&output[outputLen], data, (size_t)len);
    outputLen += len;
    return len;
}

static void AssertOutput(const char* expected)
{
    TEST_ASSERT_EQUAL_INT((int)strlen(expected), outputLen);
    TEST_ASSERT_EQUAL_MEMORY(expected, output, (size_t)outputLen);
}

static void ClearOutput(void)
{
    outputLen = 0;
    outputCalls = 0;
}

void setUp(void)
{
    PrintBuffer_Reset();
    ClearOutput();
    outputLimit = -1;
    outputResult = 0;
}

void tearDown(void)
{
}

//------------------------------------------------------------------
// Tests
//------------------------------------------------------------------
static void test_Empty_TransmitDoesNotCallOutput(void)
{
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetOccupiedSize());
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_Transmit(FakeOutput));
    TEST_ASSERT_EQUAL_INT(0, outputCalls);
}

static void test_Write_ThenTransmit_OutputsData(void)
{
    TEST_ASSERT_EQUAL_INT(5, PrintBuffer_Write("hello", 5));
    TEST_ASSERT_EQUAL_INT(5, PrintBuffer_GetOccupiedSize());

    TEST_ASSERT_EQUAL_INT(5, PrintBuffer_Transmit(FakeOutput));
    AssertOutput("hello");
    TEST_ASSERT_EQUAL_INT(1, outputCalls);
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetOccupiedSize());
}

static void test_MultipleWrites_AreOutputInOrder(void)
{
    PrintBuffer_Write("ab", 2);
    PrintBuffer_Write("cd", 2);
    PrintBuffer_Write("ef", 2);

    TEST_ASSERT_EQUAL_INT(6, PrintBuffer_Transmit(FakeOutput));
    AssertOutput("abcdef");
}

static void test_Write_InvalidArgs_WritesNothing(void)
{
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_Write(NULL, 5));
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_Write("abc", 0));
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_Write("abc", -1));
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetOccupiedSize());
}

static void test_Write_FillsToCapacity(void)
{
    const char data[] = "0123456789ABCDE"; // 15 bytes == CAPACITY
    TEST_ASSERT_EQUAL_INT(CAPACITY, PrintBuffer_Write(data, CAPACITY));
    TEST_ASSERT_EQUAL_INT(CAPACITY, PrintBuffer_GetOccupiedSize());

    TEST_ASSERT_EQUAL_INT(CAPACITY, PrintBuffer_Transmit(FakeOutput));
    AssertOutput(data);
}

static void test_Write_WhenTooLarge_TruncatesToCapacity(void)
{
    const char data[] = "0123456789ABCDEFGHIJ"; // 20 bytes
    TEST_ASSERT_EQUAL_INT(CAPACITY, PrintBuffer_Write(data, 20));

    PrintBuffer_Transmit(FakeOutput);
    AssertOutput("0123456789ABCDE");
}

static void test_Write_WhenFull_WritesNothing(void)
{
    PrintBuffer_Write("0123456789ABCDE", CAPACITY);
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_Write("X", 1));
    TEST_ASSERT_EQUAL_INT(CAPACITY, PrintBuffer_GetOccupiedSize());
}

static void test_Write_ExactlyToEndOfBuffer(void)
{
    // move head/tail to index 6, then write exactly the 10 bytes up to the end of the buffer
    PrintBuffer_Write("xxxxxx", 6);
    PrintBuffer_Transmit(FakeOutput);
    ClearOutput();

    TEST_ASSERT_EQUAL_INT(10, PrintBuffer_Write("0123456789", 10));
    TEST_ASSERT_EQUAL_INT(10, PrintBuffer_GetOccupiedSize());

    TEST_ASSERT_EQUAL_INT(10, PrintBuffer_Transmit(FakeOutput));
    AssertOutput("0123456789");
    TEST_ASSERT_EQUAL_INT(1, outputCalls);
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetOccupiedSize());

    // next write starts back at the beginning of the buffer
    PrintBuffer_Write("ok", 2);
    ClearOutput();
    PrintBuffer_Transmit(FakeOutput);
    AssertOutput("ok");
}

static void test_Write_WrapsAroundEnd(void)
{
    PrintBuffer_Write("0123456789", 10);
    PrintBuffer_Transmit(FakeOutput);
    ClearOutput();

    // 6 bytes fit before the end, 4 wrap to the start
    TEST_ASSERT_EQUAL_INT(10, PrintBuffer_Write("ABCDEFGHIJ", 10));
    TEST_ASSERT_EQUAL_INT(10, PrintBuffer_GetOccupiedSize());

    // wrapped data is output in a single transmit, using two output calls
    TEST_ASSERT_EQUAL_INT(10, PrintBuffer_Transmit(FakeOutput));
    AssertOutput("ABCDEFGHIJ");
    TEST_ASSERT_EQUAL_INT(2, outputCalls);
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetOccupiedSize());
}

static void test_Write_WhenWrapped_LimitedBySpace(void)
{
    // head at 10, tail wraps to 4 -> 10 bytes used, 5 free
    PrintBuffer_Write("0123456789", 10);
    PrintBuffer_Transmit(FakeOutput);
    PrintBuffer_Write("ABCDEFGHIJ", 10);
    ClearOutput();

    TEST_ASSERT_EQUAL_INT(5, PrintBuffer_Write("vwxyz!!", 7));
    TEST_ASSERT_EQUAL_INT(CAPACITY, PrintBuffer_GetOccupiedSize());

    PrintBuffer_Transmit(FakeOutput);
    AssertOutput("ABCDEFGHIJvwxyz");
}

static void test_Transmit_PartialOutput_KeepsRemainder(void)
{
    PrintBuffer_Write("abcdef", 6);

    outputLimit = 4;
    TEST_ASSERT_EQUAL_INT(4, PrintBuffer_Transmit(FakeOutput));
    AssertOutput("abcd");
    TEST_ASSERT_EQUAL_INT(1, outputCalls);
    TEST_ASSERT_EQUAL_INT(2, PrintBuffer_GetOccupiedSize());

    outputLimit = -1;
    TEST_ASSERT_EQUAL_INT(2, PrintBuffer_Transmit(FakeOutput));
    AssertOutput("abcdef");
}

static void test_Transmit_ZeroOutput_KeepsData(void)
{
    PrintBuffer_Write("abc", 3);

    outputLimit = 0;
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_Transmit(FakeOutput));
    TEST_ASSERT_EQUAL_INT(1, outputCalls);
    TEST_ASSERT_EQUAL_INT(3, PrintBuffer_GetOccupiedSize());
}

static void test_Transmit_Error_ReturnsErrorAndKeepsData(void)
{
    PrintBuffer_Write("abc", 3);

    outputResult = -1;
    TEST_ASSERT_EQUAL_INT(-1, PrintBuffer_Transmit(FakeOutput));
    TEST_ASSERT_EQUAL_INT(3, PrintBuffer_GetOccupiedSize());

    outputResult = 0;
    TEST_ASSERT_EQUAL_INT(3, PrintBuffer_Transmit(FakeOutput));
    AssertOutput("abc");
}

static void test_HighWaterMark_TracksMaximum(void)
{
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetHighWaterMark());

    PrintBuffer_Write("abcdefgh", 8);
    TEST_ASSERT_EQUAL_INT(8, PrintBuffer_GetHighWaterMark());

    // transmitting does not lower the high water mark
    PrintBuffer_Transmit(FakeOutput);
    TEST_ASSERT_EQUAL_INT(8, PrintBuffer_GetHighWaterMark());

    // smaller usage doesn't change it, larger usage does (including across the wrap)
    PrintBuffer_Write("abc", 3);
    TEST_ASSERT_EQUAL_INT(8, PrintBuffer_GetHighWaterMark());
    PrintBuffer_Write("defghijk", 8);
    TEST_ASSERT_EQUAL_INT(11, PrintBuffer_GetHighWaterMark());
}

static void test_Reset_ClearsDataAndHighWaterMark(void)
{
    PrintBuffer_Write("abc", 3);
    PrintBuffer_Reset();

    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetOccupiedSize());
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetHighWaterMark());
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_Transmit(FakeOutput));
}

static void test_ManyCycles_OutputMatchesInput(void)
{
    // Write varying sized chunks and transmit with varying output limits, so the head and tail wrap
    // many times at different positions. Everything accepted must come out in order, unchanged.
    char expected[sizeof(output)];
    int expectedLen = 0;
    unsigned int seed = 12345;
    char nextChar = 'a';

    outputLimit = 0;
    for (int i = 0; i < 200; i++)
    {
        seed = seed * 1103515245u + 12345u;
        const int writeLen = (int)((seed >> 16) % 9);     // 0..8 bytes
        const int limit = (int)((seed >> 8) % 7);         // 0..6 bytes per output call

        char chunk[8];
        for (int j = 0; j < writeLen; j++)
        {
            chunk[j] = nextChar;
            nextChar = (nextChar == 'z') ? 'a' : (char)(nextChar + 1);
        }

        const int written = PrintBuffer_Write(chunk, writeLen);
        TEST_ASSERT_LESS_OR_EQUAL_INT(CAPACITY, PrintBuffer_GetOccupiedSize());

        // dropped bytes are lost, so rewind the generator to keep expected data contiguous
        for (int j = written; j < writeLen; j++)
        {
            nextChar = (nextChar == 'a') ? 'z' : (char)(nextChar - 1);
        }
        memcpy(&expected[expectedLen], chunk, (size_t)written);
        expectedLen += written;

        outputLimit = limit;
        PrintBuffer_Transmit(FakeOutput);

        if (outputLen > (int)sizeof(output) - 2 * PRINTF_BUFFER_SIZE)
        {
            break; // stay within the capture buffer
        }
    }

    outputLimit = -1;
    PrintBuffer_Transmit(FakeOutput);
    TEST_ASSERT_EQUAL_INT(0, PrintBuffer_GetOccupiedSize());
    TEST_ASSERT_EQUAL_INT(expectedLen, outputLen);
    TEST_ASSERT_EQUAL_MEMORY(expected, output, (size_t)expectedLen);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Empty_TransmitDoesNotCallOutput);
    RUN_TEST(test_Write_ThenTransmit_OutputsData);
    RUN_TEST(test_MultipleWrites_AreOutputInOrder);
    RUN_TEST(test_Write_InvalidArgs_WritesNothing);
    RUN_TEST(test_Write_FillsToCapacity);
    RUN_TEST(test_Write_WhenTooLarge_TruncatesToCapacity);
    RUN_TEST(test_Write_WhenFull_WritesNothing);
    RUN_TEST(test_Write_ExactlyToEndOfBuffer);
    RUN_TEST(test_Write_WrapsAroundEnd);
    RUN_TEST(test_Write_WhenWrapped_LimitedBySpace);
    RUN_TEST(test_Transmit_PartialOutput_KeepsRemainder);
    RUN_TEST(test_Transmit_ZeroOutput_KeepsData);
    RUN_TEST(test_Transmit_Error_ReturnsErrorAndKeepsData);
    RUN_TEST(test_HighWaterMark_TracksMaximum);
    RUN_TEST(test_Reset_ClearsDataAndHighWaterMark);
    RUN_TEST(test_ManyCycles_OutputMatchesInput);
    return UNITY_END();
}
