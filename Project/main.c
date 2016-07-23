

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
void vAssertCalled( unsigned long, const char * const);

#define     STRING_LENGTH   32

// The type funcVariable is used to provide the string which should be printed and the delay time
typedef struct {
    char str[STRING_LENGTH]; // string sent to UART (shown on screen if using QEMU)
    unsigned int sleepTime;  // delay time (ms)
} funcVariable;

// The type taskVariable is used to create the task as well as send the funcVariable to the function called by task.

typedef struct {
    char name[STRING_LENGTH];   // Task name
    unsigned int stackSize;     // Stack size (byte)
    unsigned int priority;      // Priority
    funcVariable fVar;          // function Variable
} taskVariable;

// to create the task with the pointer of task handle, pointer to array of task Variable, and number of task
void createTask(xTaskHandle*, taskVariable*, int); 
// The function used by the tasks created by createTask.
void printTask(void*);

int main ( void ) {

    xTaskHandle myTaskHandle;

    taskVariable a[] = {{"Task1", 1000, 3, {"This is task 1\n", 100}},
                        {"Task2", 100, 1, {"This is task 2\n", 500}}};
                                 
                                     
    createTask(&myTaskHandle, a, sizeof(a)/sizeof(taskVariable));
                                              
    vTaskStartScheduler();


	/* Should never get here unless there was not enough heap space to create
	the idle and other system tasks. */
	return 0;
}
/*-----------------------------------------------------------*/

void createTask(xTaskHandle* myTaskHandle, taskVariable* task, int N){
    BaseType_t xReturned;
    for (int i=0; i<N; ++i) {
        xReturned = xTaskCreate(printTask,                      // Pointer to the task entry function 
        task[i].name,                   // name for the task
        task[i].stackSize/sizeof(void*),// StackDepth, sizeof(void*) indicates if it is 2/4/8 byte
        (void *) &task[i].fVar,         // variable needed by the created task.
        task[i].priority,               // Task's priority
        myTaskHandle);                  // Task handle
        if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {  // Print if tasks cannot be created.
            puts("errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY for task\n");
        }
    }
}

void printTask(void* fVar) {
    funcVariable* var = (funcVariable*) fVar;
    for( ;; ){
        printf("%s", var->str);
        vTaskDelay(var->sleepTime/portTICK_RATE_MS);            // delay 
    }
}

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
