# Printf Task
Task for handling Printf duties intended for small embedded systems

## Overview
PrintfTask is intended for embedded systems, including small micro controller environments running a lightweight RTOS (e.g. FreeRTOS). The PrintfTask offloads the majority of printf duties to a low priority thread that can run in the background. This provides more deterministic behavior when using printf in all other tasks in the system. It also provides for less timing impacts that printf can add to timing critical tasks.

## Introduction
Printf is a staple when debugging code and systems of all sizes. Often on small embedded targets, printf is routed to a UART so the user can see various output on a local terminal on their PC, or other device. Not only for debugging, but also used as an output mechanism for users, CLI (command line interface), stats, periodic info, and many other uses.  By moving the timing intensive portions to a separate task, the system can behave more deterministicly, while also providing desired output from the user without the worry of where and when to place printf statements.  It also removes the need for the user to create some custom type of print output mechanism to overcome the possible timing issues.

## Background
Nearly every product I've worked on has included some type of printf or output mechanism to a UART. This is not only useful, it's essential in many embedded projects and systems. A significant problem can arise when trying to use printf for info, stats, or debugging of timing critical tasks. Printf routed to a UART can be an expensive time operation. It can also cause different system and software behavior because of the timing impacts. Countless times I've seen a system "work" when the printf statements were included, but not when they were removed... and vice versa. This makes debugging more challenging in such instances. While it can be an indication of a poor design, it's often useful to have a printf ability with minimal disturbance to the system timing.  This solution proposes one such implementation to minimize timing issues for printf

## Features
- Use normal printf (No need to write a custom printf implementation, or replace all printf statements already being used)
- Utilizes circular buffer that can be any size desired by the user
- Dedicated printf task handles output operations (e.g. UART output)
- Significantly reduces time for printf calls


## Using the Code
There are only two basic functions needed to utilize the functionality within existing tasks/threads.

First, the task/thread needs to register itself with the Task monitor. This should be done as part of the initialization of the task/thread, or at the start of task/thread.

- The first argument is the maximum time the task monitor will wait for a response from a task, from a checkin request.
- The second argument is the function pointer to the checkin call the task monitor uses to send a message to the task when a checkin request is issued.

```
TaskMonitor::Register(TIMEOUT, &TaskCheckin);
```
The callback in the `Register` function above should be something that can be used to notify the task registering with the task monitor.  For example:

```
void Task1::TaskCheckin()
{
    Message msg(Message::TASK_CHECKIN);
    ASSERT(PASS == QSend(msgQHandle, &msg));
} 
```

The `TaskCheckin` will be called by the task monitor task when it requests a checkin from `Task1`. This implementation will create a message to put in the message queue that Task1 listens on and take appropriate action on receiving that message (e.g. call `TaskCheckin()` as shown below).

For another solution without relying on message queue in `Task1`, see OS Porting below.

The other main function is to checkin when desired with the task monitor.

```
TaskMonitor::TaskCheckin();
```
The following sequence diagram shows how this process operates.
![image](https://user-images.githubusercontent.com/26239627/198851117-43866cd8-f42d-49a7-b140-38054fe01c95.png)

A task can respond to a checkin request, or preemptively checkin as desired.

The task monitor will send a request to the registered task once every timeout period (`TIMEOUT` time provided during registration). If the registered task posts a checkin to the task monitor before a request has been sent by the task monitor, then the timer will reset and the task monitor will not send a request until another full timeout period has elapsed.

### Task Monitor Task
The task monitor functionality relies on the operating system in use and is required to have its own task/thread to operate in. Therefore, the developer needs to create and start the task monitor task as part of the rest of the system task initializations. For example, in FreeRTOS:

```
void StartTaskMonitor(void *argument)
{
    TaskMonitor taskMon;
    taskMon.Initialize();
    taskMon.Run();
}

void app_main(void)
{
    TaskHandle_t taskHandle = nullptr;

    // Task monitor
    xTaskCreate(&StartTaskMonitor, "TaskMonitor", TASK_MONITOR_STACK_SIZE, 
                NULL, TASK_MONITOR_PRIORITY, &taskHandle);
}
```

### OS Porting
The task monitor also uses some of the operating system calls to operate. Therefore, *TargetPort.hpp* and *TargetPort.cpp* were created to easily port for different operating systems being utilitized. The main items for porting purposes are message queue operations, task identification operations and watchdog operations.

#### Alternative TaskCheckin Callback Solution
If a task in the system does **not** utilize a message queue, you can use a static boolean to accomplish the same outcome. The callback function could look like something below:

```
static bool checkinRequested = false;

void Task1::TaskCheckin() 
{ 
   checkinRequested = true;
} 
```
In the task execution loop, you can simply check if this flag is set, send the checkin message to the TaskMonitor, and continue the task execution.  An example of this is below:

```
void Task1::Run()
{
   // Task loop - never exits
   while (true)
   {
      if (true == checkinRequested)
      {
         TaskMonitor::TaskCheckin();
         checkinRequested = false;
      }

      // Rest of Task execution
      // ...
   }
}
```
### Violation Actions
The task monitor takes an action when it determines that a task has violated the time limit to checkin. The default action is to do the following:

```
// check for expired task checkins... if there was a violation, 
// we can print the name of the violator and then wait until the watchdog bites
if (monInfo.elapsedTimeWaitingForResponse > monInfo.timeout)
{
   LOG_ERROR("TaskMonitor", "Task: %s - Exceeded Checkin Time. 
              Elapsed time %u ms, Timeout %u ms",
              GetTaskName(taskId), monInfo.elapsedTimeWaitingForResponse, 
              monInfo.timeout);

   while(1) {}
}
```
The `while(1) {}` allows the task to spin forever until the watchdog resets the device. This code can be updated to do whatever the user desires:

```
// Designers can also replace the above with their own logic that can reset tasks, 
// end tasks, or take other actions based on violations. Example below:
// if (monInfo.elapsedTimeWaitingForResponse > monInfo.timeout)
// {
//    // Do action, such as restart the task that violated the timeout
// }
```
The included code provides more thorough examples and details.

## Targets
I started this work on an STM32 board, but was required to switch to the ESP32 platform before completing the project.  I felt it was valuable to provide both as different examples of doing essentially the same thing.  The ESP32 is the most complete. However, this can be adapted to use with any target and RTOS.
