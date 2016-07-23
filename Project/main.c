

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

/*
 * Prototypes for the standard FreeRTOS callback/hook functions implemented
 * within this file.
 */
void vApplicationMallocFailedHook( void );

static void task1(void* pParams) {
	TickType_t xNextWakeTime;
	const TickType_t xCycleFrequency = pdMS_TO_TICKS( 100UL );

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	/* Just to remove compiler warning. */
	( void ) pParams;

	for(;;) {

		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, xCycleFrequency );

		printf("This is task 1\n");
		fflush(stdout);

	}
}

static void task2(void* pParams) {
	TickType_t xNextWakeTime;
	const TickType_t xCycleFrequency = pdMS_TO_TICKS( 500UL );

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	/* Just to remove compiler warning. */
	( void ) pParams;

	for(;;) {

		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, xCycleFrequency );

		printf("This is task 2\n");
		fflush(stdout);

	}
}


/*-----------------------------------------------------------*/

int main ( void )
{

	xTaskCreate(task1,
		"Task 1",
		1000,
		NULL,
		tskIDLE_PRIORITY + 1,
		NULL
		);

	xTaskCreate(task2,
		"Task 2",
		100,
		NULL,
		tskIDLE_PRIORITY + 1,
		NULL
		);

	/* Start the scheduler itself. */
	vTaskStartScheduler();

	/* Should never get here unless there was not enough heap space to create
	the idle and other system tasks. */
	return 0;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
 	taskENTER_CRITICAL();
	{
        printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
        fflush(stdout);
	}
	taskEXIT_CRITICAL();
	exit(-1);
}
/*-----------------------------------------------------------*/
