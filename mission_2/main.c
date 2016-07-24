/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#define SIZE 10
#define ROW SIZE
#define COL SIZE

/*
 * Prototypes for the standard FreeRTOS callback/hook functions implemented
 * within this file.
 */
void vApplicationMallocFailedHook( void );
void vAssertCalled( unsigned long, const char * const);
void vApplicationTickHook( void );
static void matrix_task(void);
static void communication_task(void);
static void prioritySet_task(void);
static TickType_t communicationTime, communicationDuration;
static TickType_t matrixTime, matrixDuration;

xTaskHandle matrix_handle, communication_handle, priority_handle;

int main ( void ) {
    xTaskCreate((pdTASK_CODE)prioritySet_task, (char *)"Priority", configMINIMAL_STACK_SIZE, NULL, 5, &priority_handle);
    xTaskCreate((pdTASK_CODE)communication_task, (char *)"Communication", configMINIMAL_STACK_SIZE, NULL, 1, &communication_handle);
    xTaskCreate((pdTASK_CODE)matrix_task, (char *)"Matrix", 1000, NULL, 3, &matrix_handle);
    vTaskStartScheduler();
	/* Should never get here unless there was not enough heap space to create
	the idle and other system tasks. */
	return 0;
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

static void prioritySet_task(void){
    for(;;) {
        if (communicationDuration > 1000) {
            vTaskPrioritySet(communication_handle, 4);
        } else if (communicationDuration < 200) {
            vTaskPrioritySet(communication_handle, 2);
        }
        vTaskDelay(200);
    }
}
static void matrix_task(void) {
    int i;
	double **a = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) a[i] = (double *)pvPortMalloc(COL * sizeof(double));
	double **b = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) b[i] = (double *)pvPortMalloc(COL * sizeof(double));
	double **c = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) c[i] = (double *)pvPortMalloc(COL * sizeof(double));

	double sum = 0.0;
	int j, k, l;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			a[i][j] = 1.5;
			b[i][j] = 2.6;
		}
	}

	while (1) {
		/*
		* In an embedded systems, matrix multiplication would block the CPU for a long time
		* but since this is a PC simulator we must add one additional dummy delay.
		*/
        matrixTime = xTaskGetTickCount();
		long simulationdelay;
		for (simulationdelay = 0; simulationdelay<1000000000; simulationdelay++)
			;
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				c[i][j] = 0.0;
			}
		}

		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				sum = 0.0;
				for (k = 0; k < SIZE; k++) {
					for (l = 0; l<10; l++) {
						sum = sum + a[i][k] * b[k][j];
					}
				}
				c[i][j] = sum;
			}
		}
		vTaskDelay(100);
        matrixDuration = xTaskGetTickCount() - matrixTime;
        printf("Matrix Time: %d\n",matrixDuration);  // Only for debug
        fflush(stdout);
	}
}

static void communication_task(void) {
    while (1) {
        communicationTime = xTaskGetTickCount();
        printf("Sending data...\n");
        fflush(stdout);
        vTaskDelay(100);
        printf("Data sent!\n");
        fflush(stdout);
        vTaskDelay(100);
        communicationDuration = xTaskGetTickCount() - communicationTime;
        printf("Communication Time: %d\n",communicationDuration);  // Only for debug
        fflush(stdout);
    }
}

/*
void vApplicationTickHook( void ) {
}*/
