/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "Application.h"
#include "zork_config.h"


xTaskHandle ZorkTaskHndl = NULL;

static void BlinkyTask(void *pvParameters){
	for(;;){
		LED1_Neg();
	    SHELL_SendString('I am Blinky');
	    WAIT1_Waitms(100);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

static void BeepyTask(void *pvParameters){
	for(;;){
	    BUZ_Beep(100,2);
	    SHELL_SendString('I am Beepy');
	    WAIT1_Waitms(100);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

static void ZorkTask (void){
	vTaskSuspend(ZorkTaskHndl);
	zork_config();
	run_zork_game();
	for(;;){
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}


void RTOS_Init(void) {
  /*! \todo Create tasks here */
	  /*BaseType_t blink_res;
	  blink_res = xTaskCreate(BlinkyTask,
			  	  "Blinky",
				  500/sizeof(StackType_t),
				  (void*)NULL,
				  tskIDLE_PRIORITY+1,
				  NULL
	  );
	  if (blink_res != pdPASS){
			  // something went wrong
			  WAIT1_Waitms(10);
	  }
	  BaseType_t beep_res;
	  beep_res = xTaskCreate(BeepyTask,
			  	 "Beepy",
				 500/sizeof(StackType_t),
				 (void*)NULL,
				 tskIDLE_PRIORITY+1,
				 NULL
	  );
	  if (beep_res != pdPASS){
			  // something went wrong
			  WAIT1_Waitms(10);
	  }*/


   	BaseType_t zork_res;
   	zork_res = xTaskCreate(ZorkTask,
   			  "Zork",
   			  1000/sizeof(StackType_t),
   			  (void*)NULL,
   			  tskIDLE_PRIORITY+1,
   			  &ZorkTaskHndl
   			  );
	  if (zork_res != pdPASS){
			  // something went wrong
			  WAIT1_Waitms(10);
	  }
}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}

#endif /* PL_CONFIG_HAS_RTOS */
