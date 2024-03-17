#include "PrintfTask.h"
#include "cmsis_os.h"

extern "C" {
void printfTaskRun(void *argument)
{
	PrintfTask task;
	task.run();
}

void task1Run(void *argument)
{
	printf("Task 1 started\n");
	while (true) {
		printf("Task 1 is outputting some data using printf\n");
		osDelay(100);
	}
}

void task2Run(void *argument)
{
	while (true) {
		printf("Task 2 is outputting some data using printf\n");
		osDelay(100);
	}
}
}

