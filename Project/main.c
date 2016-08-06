/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

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

int iMatrix = 0, iMatrix_temp = 0, iMatrix_resTime = 0;
int iAperiod = 0, iAperiod_temp = 0, iAperiod_resTime = 0;
TimerHandle_t xTimer;
void vTimerCallback(TimerHandle_t pxTimer);
xTaskHandle matrix_handle, aperiodic_handle;
static void aperiodic_task(void);
long lExpireCounters = 0;
int main ( void ) {


    xTimer = xTimerCreate("Timer", // Just a text name, not used by the RTOS kernel. 
            pdMS_TO_TICKS( 5000 ), // The timer period in ticks, must be greater than 0. 
                           pdTRUE, // The timers will auto-reload themselves when they expire. 
                        (void*) 0,// The ID is used to store a count of the number of times the timer has expired. 
                  vTimerCallback);// Each timer calls the same callback when it expires. 

    if( xTimer == NULL ) puts("Not create!");
    else if( xTimerStart( xTimer, 0 ) != pdPASS ) puts("Not active!");

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
        iMatrix = -1;
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
        iMatrix = 1;
		vTaskDelay(100);
	}
}

void vTimerCallback(TimerHandle_t pxTimer) {
    printf("Timer callback!\n");
    xTaskCreate((pdTASK_CODE)aperiodic_task, (char *)"Aperiodic", configMINIMAL_STACK_SIZE, NULL, 4, &aperiodic_handle);
    //long lArrayIndex;
    const long xMaxExpiryCountBeforeStopping = 10;
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(pxTimer);
    /* Increment the number of times that pxTimer has expired. */
    lExpireCounters += 1;
    /* If the timer has expired 10 times then stop it from running. */
    if (lExpireCounters == xMaxExpiryCountBeforeStopping) {
        /* Do not use a block time if calling a timer API function from a
        timer callback function, as doing so could cause a deadlock! */
        xTimerStop(pxTimer, 0);
    }
}


static void aperiodic_task(void){
    printf("Aperiodic task started!\n");
    fflush(stdout);
    long i;
    //iAperiod = xTaskGetTickCount();
    iAperiod = -1;
    for (i = 0; i<1000000000; i++); //Dummy workload
    printf("Aperiodic task done!\n");
    fflush(stdout);
    //iAperiod_resTime = xTaskGetTickCount() - iAperiod;
    iAperiod = 1;
    //printf("Aperiod response time: %d\n", iAperiod_resTime/portTICK_PERIOD_MS);
    vTaskDelete(aperiodic_handle);
}

void vApplicationTickHook( void ) {
    if (iMatrix == -1) ++iMatrix_temp;
    if (iMatrix_temp > 0 && iMatrix == 1) {
        iMatrix_resTime = iMatrix_temp;
        iMatrix_temp = 0;
        printf("Matrix response time: %d\n", iMatrix_resTime);
    }
    
    if (iAperiod == -1) ++iAperiod_temp;
    if (iAperiod_temp > 0 && iAperiod == 1) {
        iAperiod_resTime = iAperiod_temp;
        iAperiod_temp = 0;
        printf("Aperiod response time: %d\n", iAperiod_resTime/portTICK_PERIOD_MS);
    }
}
