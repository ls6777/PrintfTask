# Printf Task
Task for handling `printf` duties intended for small embedded systems

## Overview
`PrintfTask` is intended for embedded systems, including small micro controller environments running a lightweight RTOS (e.g. FreeRTOS). The PrintfTask offloads the majority of `printf` duties to a low priority thread that can run in the background. This provides more deterministic behavior when using `printf` in all other tasks in the system. It also provides for less timing impacts that `printf` can add to timing critical tasks.

## Introduction
`printf` is a staple when debugging code and systems of all sizes. Often on small embedded targets, `printf` is routed to a UART so the developer can see various output on a local terminal on their PC, or other device. Not only for debugging, but also used as an output mechanism for users, CLI (command line interface), stats, periodic info, and many other uses.  By moving the timing intensive portions to a separate task, the system can behave more deterministically, while also providing desired output to the user without the worry of where and when to place `printf` statements.  It also removes the need for the developer to create some custom type of print output mechanism to overcome the possible timing issues.

## Background
Nearly every product I've worked on has included some type of `printf` or output mechanism to a UART. This is not only useful, it's essential in many embedded projects and systems. A significant problem can arise when trying to use `printf` for info, stats, or debugging of timing critical tasks. `printf` routed to a UART can be an expensive time operation. It can also cause different system and software behavior because of the timing impacts. Countless times I've seen a system "work" when the `printf` statements were included, but not when they were removed... and vice versa. This makes debugging more challenging in such instances. While it can be an indication of a poor design, it's often useful to have a `printf` ability with minimal disturbance to the system timing.  This solution proposes one such implementation to minimize timing issues for `printf`.

## Features
- Use normal `printf` (No need to write a custom `printf` implementation, or replace all `printf` statements already being used)
- Utilizes circular buffer that can be any size desired by the developer
- Dedicated `printf` task handles output operations (e.g. UART output)
- Significantly reduces time for `printf` calls
- Provides more deterministic behaviour for all `printf` calls
- Minimizes timing impacts to timing critical tasks

## Using the Code
For usage throughout the code, just use `printf` as you normally would. Once setup and configured, `printf` can be added anywhere without any special considerations.

### Printf Task
The `PrintfTask` functionality relies on the operating system in use and is required to have its own task/thread to operate in. Therefore, the developer needs to create and start the `PrintfTask` as part of the rest of the system task initializations. For example, in FreeRTOS:

```
void StartPrintfTask(void *argument)
{
    PrintfTask task;
    task.Initialize();
    task.Run();
}

void app_main(void)
{
    // Printf task
    xTaskCreate(&StartPrintfTask, "PrintfTask", PRINTF_TASK_STACK_SIZE, nullptr, PRINTF_TASK_PRIORITY, nullptr);

    // Other initialization
    // ...
}
```

### Project Layout
| File | Purpose |
| --- | --- |
| *main/Inc/PrintBuffer.hpp* | Target independent circular buffer class template (`PrintBuffer<Size>`) that holds `printf` output. No OS or C library specifics, so it is unit tested on a PC. |
| *main/Src/PrintUtils.cpp* | Target independent `stdout` API. Owns the single printf buffer and connects it to `stdout` (`PrintUtils::InitStdOut`, `Write`, `TransmitData`, `GetHighWaterMark`). |
| *main/Src/StdOutPort.cpp* | **Target specific.** Redirects `stdout` into the buffer and writes buffered data to the real output device. The only file to change for a new target / C library. |
| *main/Src/PrintfTask.cpp* | The low priority task that periodically transmits the buffered data. |
| *main/Src/TargetPort.cpp* | OS abstraction (message queues, delays) for porting to other RTOSes. |
| *main/Src/Task1.cpp*, *main/Src/Task2.cpp* | Example tasks used for the timing measurements below. |
| *test/* | Host unit tests. |

The buffer size defaults to 15000 bytes and can be changed with `PrintUtils::BUFFER_SIZE` in *PrintUtils.hpp*. `PrintUtils::GetHighWaterMark()` reports the most of the buffer ever used, which helps choose an appropriate size.

### OS Porting
The `PrintfTask` also uses some of the operating system calls to operate. Therefore, *TargetPort.hpp* and *TargetPort.cpp* were created to easily port for different operating systems being utilized. The main items for porting purposes are message queue operations. The functions are static members of the `TargetPort` class (e.g. `TargetPort::QSend()`), and the types, constants and macros are global.

### Target Porting
This code was developed on an ESP32. However, it can be adapted to use with any target and RTOS. Everything target specific about `stdout` is in *StdOutPort.cpp*, which implements the two functions declared in the static class in *StdOutPort.hpp*:

```cpp
class StdOutPort
{
    public:
        // Redirect stdout so data written by printf goes into the printf buffer (PrintUtils::Write)
        static void Redirect();

        // Write data to the real stdout device (e.g. UART)
        static int Write(const char* data, int len);
};
```

For the ESP32, the C library's stdout write function pointer is saved and replaced with one that writes into the buffer. The saved function is then used to output the data when the `PrintfTask` runs. *StdOutPort.cpp* supports both C libraries available in ESP-IDF, selected by `CONFIG_LIBC`.

Picolibc (the default since ESP-IDF v6.0), where `stdout` is a buffered file that writes through a file descriptor:

```cpp
ssize_t WriteReroute(int fd, const void* data, size_t len)
{
    return PrintUtils::Write(static_cast<const char*>(data), static_cast<int>(len));
}

void StdOutPort::Redirect()
{
    auto* const bufio = reinterpret_cast<__file_bufio*>(stdout);

    stdOutFd = static_cast<int>(reinterpret_cast<intptr_t>(bufio->ptr));
    realStdOutWrite = bufio->write_int;
    bufio->write_int = WriteReroute;
}

int StdOutPort::Write(const char* data, int len)
{
    return static_cast<int>(realStdOutWrite(stdOutFd, data, static_cast<size_t>(len)));
}
```

Newlib (ESP-IDF v5.x, or `CONFIG_LIBC_NEWLIB`):

```cpp
int WriteReroute(_reent* r, void* cookie, const char* data, int len)
{
    return PrintUtils::Write(data, len);
}

void StdOutPort::Redirect()
{
    realStdOutWrite = _GLOBAL_REENT->_stdout->_write;
    _GLOBAL_REENT->_stdout->_write = WriteReroute;
}

int StdOutPort::Write(const char* data, int len)
{
    return realStdOutWrite(_GLOBAL_REENT, _GLOBAL_REENT->_stdout, data, len);
}
```

How `stdout` works and is assigned will vary by target, so the developer will need to research how to do this based on the desired target. For example, on an STM32, using the STM32IDE and compiler, `StdOutPort::Redirect` can be empty and the C library's `_write` syscall overridden to call `PrintUtils::Write`, while `StdOutPort::Write` sends the data with the UART driver. `_write` is called by the C library, so it must be declared `extern "C"`:

```cpp
extern "C" int _write(int fd, char* ptr, int len)
{
    (void)fd;
    return PrintUtils::Write(ptr, len);
}

void StdOutPort::Redirect()
{
}

int StdOutPort::Write(const char* data, int len)
{
    const auto bytes = reinterpret_cast<const uint8_t*>(data);
    return (HAL_UART_Transmit(&huart2, bytes, len, HAL_MAX_DELAY) == HAL_OK) ? len : -1;
}
```

### Multithreaded Protection
On the ESP32, both Picolibc and Newlib lock `stdout` while calling the write function, so writes from multiple tasks are already serialized. Therefore, the code provided doesn't use locks/mutexes for the `WriteReroute` function in *StdOutPort.cpp*. If the target does **not** support thread protection natively, then a lock or mutex should be used around the call to `PrintUtils::Write` in the redirected write. This will keep the `printf` statements, from multiple tasks, from interspersing or corrupting the stdout buffer.

## Building
Build and flash with ESP-IDF v6.1 (v5.x is also supported):

```
idf.py build
idf.py -p <PORT> flash monitor
```

### VS Code
The project is set up for VS Code. When you open the folder, VS Code offers to install the recommended extensions: [ESP-IDF](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension) and [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools).

1. In the ESP-IDF extension, select your ESP-IDF install, the target (`esp32`) and the serial port. These are personal settings, so they're saved to *.vscode/settings.json*, which isn't committed.
2. Use **Build**, **Flash** and **Monitor** (or **Build, Flash and Monitor**) from the ESP-IDF status bar.
3. The C/C++ extension reads *build/compile_commands.json* (and *test/build/compile_commands.json* for the unit tests), so IntelliSense, the outline and go to definition work after the first build.

## Unit Tests
The target independent code (*PrintBuffer.hpp* and *PrintUtils.cpp*, using a fake `StdOutPort`) is tested on a PC using the [Unity](https://github.com/ThrowTheSwitch/Unity) test framework that ships with ESP-IDF. The buffer tests use a small `PrintBuffer<16>`, recreated for each test, so wrap-around and full-buffer cases are easy to exercise.

Requirements: a host C and C++ compiler (e.g. gcc/g++ from MinGW), CMake, Ninja (CMake and Ninja are included with ESP-IDF) and `IDF_PATH` set (or pass `-DUNITY_DIR=<path to Unity src>`).

```
cmake -S test -B test/build -G Ninja
cmake --build test/build
ctest --test-dir test/build --output-on-failure
```

## Analysis
Based on the ESP32 target, we can inspect and do measurements to see the improvements made for this implementation.

The measurements were made on an ESP-WROVER-KIT (ESP32-D0WDQ6) running ESP-IDF v6.1, with `stdout` on the UART at 115200 baud. A Saleae Logic Pro 8 sampling at 100 MS/s recorded GPIO18 and GPIO19, which are set high while the measured `printf` statements run. Each "normal `printf`" build is the same firmware with the `PrintUtils::InitStdOut()` call removed, so `printf` writes straight to the UART. The statistics cover every pulse in a 10 second (Example 1) or 14 second (Example 2) capture.

### Example 1
Let's start by looking at a simple single line example:

```
void Task1::HandleProcess()
{
    static uint32_t count = 0;

    count++;

    gpio_set_level(GPIO_OUTPUT_IO_0, 1);
    printf("Task1 stuff: %lu\r\n", (unsigned long)count);
    gpio_set_level(GPIO_OUTPUT_IO_0, 0);

    DELAY_MS(10);
    Process();
}
```

The above simply writes a single `printf` output statement and delays 10 ms. This is repeated forever.  The GPIO setting allows us to independently measure the results, with and without, the `PrintfTask` and `printf` redirects.

With normal `printf` implementation, we see the following results:

![image](./docs/Task1PrintfTimeNormal.png)

![image](./docs/Task1PrintfTimeNormalMinMaxAvg.png)

And with `PrintfTask` implementation:

![image](./docs/Task1PrintfTimeBuffer.png)

![image](./docs/Task1PrintfTimeBufferMinMaxAvg.png)

A typical `printf` drops from 65 microseconds to 32 microseconds, about a 2x improvement. On its own that may not have a significant timing impact on your system. The single statement is small enough to fit in the UART's buffer, so the UART holds the data while it's sent out on the bus and the CPU continues on to further operations.

The bigger difference is the worst case. With the normal `printf`, a few calls took up to 19.3 milliseconds. These happen every 5 seconds, when Task2 prints its task statistics: Task2's output fills the UART, so Task1's `printf` has to wait for the UART to drain, even though Task1 has the higher priority. With the `PrintfTask`, the slowest of the 1001 calls took 33.3 microseconds, because writing to the buffer never waits for the UART.

### Example 2
Let's look at a more complex example with multiple `printf` statements:

```
void Task2::HandleProcess()
{
    vTaskList(taskListBuf);
    gpio_set_level(GPIO_OUTPUT_IO_0, 1);
    printf("\r\nTASK INFO**********************************************\r\n");
    printf("Name          State  Priority   Stack   Num    Core\r\n");
    printf("*******************************************************\r\n");
    printf("%s\r\n", taskListBuf);
    printf("\r\n");
    gpio_set_level(GPIO_OUTPUT_IO_0, 0);

    vTaskGetRunTimeStats(taskStatsBuf);
    gpio_set_level(GPIO_OUTPUT_IO_1, 1);
    printf("TASK STATS INFO****************************************\r\n");
    printf("Name             Abs Time       %% Time\r\n");
    printf("*******************************************\r\n");
    printf("%s\r\n", taskStatsBuf);
    printf("\r\n");

    printf("High Water Mark: %d\r\n", PrintUtils::GetHighWaterMark());
    printf("\r\n");
    gpio_set_level(GPIO_OUTPUT_IO_1, 0);
}
```

The above writes multiple `printf` output statements. For these measurements Task2 ran every 200 ms (the example code runs it every 5 seconds). This is repeated forever. There are two separate sections based on getting task list and task stats. The GPIO setting allows us to independently measure the results, with and without, the `PrintfTask` and `printf` redirects.

With normal `printf` implementation, we see the following results:

Task List:

![image](./docs/Task2TaskListPrintfTimeNormal.png)

![image](./docs/Task2TaskListPrintfTimeNormalMinMaxAvg.png)

Task Stats:

![image](./docs/Task2TaskStatsPrintfTimeNormal.png)

![image](./docs/Task2TaskStatsPrintfTimeNormalMinMaxAvg.png)

And with `PrintfTask` implementation:

Task List:

![image](./docs/Task2TaskListPrintfTimeBuffer.png)

![image](./docs/Task2TaskListPrintfTimeBufferMinMaxAvg.png)

Task Stats:

![image](./docs/Task2TaskStatsPrintfTimeBuffer.png)

![image](./docs/Task2TaskStatsPrintfTimeBufferMinMaxAvg.png)

Now we can see a significant improvement using the `PrintfTask` implementation. The task list section drops from 26.3 milliseconds to 271 microseconds, and the task stats section from 36.0 milliseconds to 290 microseconds. This is roughly a 100x improvement and may have a large timing impact for timing critical tasks. The UART buffer can not hold all of the data that the multiple `printf` statements are producing quickly. As such, the `printf` statements for the normal `printf` implementation must wait for the UART to transmit much of the data before allowing the CPU to continue to the next operations. The `PrintfTask` can store all the data in the buffer quickly and then output the data when the `PrintfTask` is allowed to execute.

## Alternative Implementations
There are alternative implementations that can accomplish similar results. Each has its own advantages and disadvantages. I will briefly review a few of these alternatives.

### Overriding `printf`
Instead of redirecting, overriding `printf` could be another tactic. With a custom `printf`, you could do something like just reroute to a buffer using `vsprintf` and then printing that buffer out later. This is similar to what the code here does, but at a higher level. The problem with this approach is properly overriding `printf`, if even possible, and also remembering to `#include` the correct file for this approach. There's more opportunity for human error and making mistakes.

### Creating Custom Print function
It's possible to also just create a custom function, let's call it `MyPrintf`. This can also use `vsprintf`, or some other buffer writing mechanism. We run into similar issues as above though. Every file using this needs to `#include` the proper file. This is less likely to be have conflicts or mistakes because the name is different. However, if you're adding this to an existing system, you may have to replace ALL `printf` statements in your code. That may be unreasonable or not possible in many projects.

### UART interrupts
If `stdout` is routed to the UART through a UART driver, specifically a custom driver, the UART driver could hold the buffer. In this implementation, the UART generates an interrupt when it can receive more data. The ISR can move one, or more bytes, from the driver buffer into the UART hardware buffer. This allows the tasks to keep executing while offloading the communication transmission. The negatives with this approach are added interrupts in the system (which could cause less deterministic behavior) and this is specific to UART usage only. It's also only available if the UART driver can be customized and used for routing `stdout`.

## Conclusions
The included code demonstrates a small improvement in performance, in the worst case, and a **large** improvement in best case usage scenarios. The developer should evaluate if this is appropriate, or even one of the alternative approaches, to improve deterministic behavior in the intended system. This change has the potential to allow `printf` in all tasks without concern for timing impacts, while providing more useful output for the developer and/or user.


