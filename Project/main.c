/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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
static void matrixRead_task(void);

xTaskHandle matrix_handle, matrixRead_handle;
xQueueHandle xQueue;

int main ( void ) {
    xQueue = xQueueCreate(10, sizeof(double*) );
    xTaskCreate((pdTASK_CODE)matrix_task, (char *)"Matrix", 1000, NULL, 3, &matrix_handle);
    xTaskCreate((pdTASK_CODE)matrixRead_task, (char *)"Matrix_Read", 1000, NULL, 3, &matrixRead_handle);
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

static void matrixRead_task(void) {
    double **c = (double **)pvPortMalloc(ROW * sizeof(double*));;
    unsigned int nSample = 0;
    for (;;) {
        if (xQueueReceive(xQueue, &c, portMAX_DELAY)) {
            printf("Sample #%u\n", nSample++);
            for (int i=0; i<ROW; ++i) {
                for (int j=0; j<COL; ++j) {
                    printf("%4.2f\t", c[i][j]);
                }
                puts("");
            }
        }
    }
}

static void matrix_task(void) 
{
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
        /** In an embedded systems, matrix multiplication would block the CPU for a long time
          * but since this is a PC simulator we must add one additional dummy delay.*/
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
        xQueueSendToBack(xQueue, &c, 0);
        vTaskDelay(100);
    }
}



/*
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
}*/
