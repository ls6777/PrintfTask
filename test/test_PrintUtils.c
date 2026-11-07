/// @file test_PrintUtils.c
/// @brief Host unit tests for PrintUtils.c, using a fake StdOutPort.

#include "unity.h"
#include "PrintUtils.h"
#include "PrintBuffer.h"
#include "StdOutPort.h"

#include <string.h>

//------------------------------------------------------------------
// Fake StdOutPort
//------------------------------------------------------------------
static int redirectCalls;
static char output[256];
static int outputLen;
static int writeResult; // if non-zero, returned instead of writing (e.g. an error)

void StdOutPort_Redirect(void)
{
    redirectCalls++;
}

int StdOutPort_Write(const char* data, int len)
{
    if (writeResult != 0)
    {
        return writeResult;
    }
    memcpy(&output[outputLen], data, (size_t)len);
    outputLen += len;
    return len;
}

void setUp(void)
{
    PrintBuffer_Reset();
    redirectCalls = 0;
    outputLen = 0;
    writeResult = 0;
}

void tearDown(void)
{
}

//------------------------------------------------------------------
// Tests
//------------------------------------------------------------------
static void test_InitStdOut_RedirectsStdOut(void)
{
    InitStdOut();
    TEST_ASSERT_EQUAL_INT(1, redirectCalls);
}

static void test_TransmitData_WritesBufferedDataToPort(void)
{
    PrintBuffer_Write("hello", 5);

    TEST_ASSERT_EQUAL_INT(5, TransmitData());
    TEST_ASSERT_EQUAL_INT(5, outputLen);
    TEST_ASSERT_EQUAL_MEMORY("hello", output, 5);
}

static void test_TransmitData_Empty_ReturnsZero(void)
{
    TEST_ASSERT_EQUAL_INT(0, TransmitData());
    TEST_ASSERT_EQUAL_INT(0, outputLen);
}

static void test_TransmitData_PortError_ReturnsError(void)
{
    PrintBuffer_Write("hello", 5);
    writeResult = -1;

    TEST_ASSERT_EQUAL_INT(-1, TransmitData());
    TEST_ASSERT_EQUAL_INT(5, PrintBuffer_GetOccupiedSize());
}

static void test_GetHighWaterMark_ReturnsBufferHighWaterMark(void)
{
    PrintBuffer_Write("abcdefg", 7);
    TransmitData();

    TEST_ASSERT_EQUAL_INT(7, GetHighWaterMark());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_InitStdOut_RedirectsStdOut);
    RUN_TEST(test_TransmitData_WritesBufferedDataToPort);
    RUN_TEST(test_TransmitData_Empty_ReturnsZero);
    RUN_TEST(test_TransmitData_PortError_ReturnsError);
    RUN_TEST(test_GetHighWaterMark_ReturnsBufferHighWaterMark);
    return UNITY_END();
}
