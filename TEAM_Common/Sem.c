/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */


#include "Platform.h" /* interface to the platform */
#if PL_CONFIG_HAS_SEMAPHORE
#include "FRTOS1.h"
#include "task.h"
#include "Sem.h"
#include "LED.h"
TaskHandle_t Hndl_MasterTask, Hndl_SlaveTask;
xSemaphoreHandle sem = NULL;

static void vSlaveTask(void *pvParameters) {
	SemaphoreHandle_t sem = *(SemaphoreHandle_t*)pvParameters;
	for(;;){
		xSemaphoreTake(sem,pdMS_TO_TICKS(10000));
		LED1_Neg();
	}
  /*! \todo Implement functionality */
}

static void vMasterTask(void *pvParameters) {
	SemaphoreHandle_t sem;
	sem = xSemaphoreCreateBinary();
	if(sem==NULL){
		for(;;);
	}
	if(xTaskCreate(vSlaveTask,"SemaphoreSlave",500/sizeof(StackType_t), &sem, tskIDLE_PRIORITY+2, Hndl_MasterTask)!= pdPASS){
			 for(;;);// something went wrong
		}
	for(;;){
		xSemaphoreGive(sem);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
  /*! \todo send semaphore from master task to slave task */
}

void SEM_Deinit(void) {
	vTaskDelete(vSlaveTask);
	vTaskDelete(vMasterTask);
}

/*! \brief Initializes module */
void SEM_Init(void) {

	if(xTaskCreate(vMasterTask,"SemaphoreMaster",800/sizeof(StackType_t), (void*)NULL, tskIDLE_PRIORITY+2, &Hndl_MasterTask)!= pdPASS){
		  // something went wrong
		 for(;;);
	}
}
#endif /* PL_CONFIG_HAS_SEMAPHORE */
