/* ###################################################################
**     Filename    : main.c
**     Project     : K64F-FreeRTOS
**     Processor   : MK64FN1M0VLL12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-05-11, 11:50, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Pins1.h"
#include "FRTOS1.h"
#include "UTIL1.h"
#include "LED_Blue.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "LED_Red.h"
#include "LEDpin2.h"
#include "BitIoLdd2.h"
#include "LED_Green.h"
#include "LEDpin3.h"
#include "BitIoLdd3.h"
#include "MCUC1.h"
#include "PTRC1.h"
#include "SYS1.h"
#include "RTT1.h"
#include "WAIT1.h"
#include "CLS1.h"
#include "XF1.h"
#include "CS1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "semphr.h"
#include <stdbool.h>

void Delay_Nonsense(uint32_t * DelayCounter, uint32_t const * TargetCount);

/* Create a semaphore to synchronize the LED tasks */
SemaphoreHandle_t LEDMutex;

void Led_BlueBlink(void *pvParameters)
{
   const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
   uint32_t BlueDelay = 0;
   const uint32_t TargetCount = 16000;

   for(;;)
   {
	   for(int i = 0; i < 10; i++)
	   {
		  vTaskDelay(xDelay);
	   }

	   // Obtain the Mutex - block for 0 ticks if the semaphore is not
	   // immediately available.
	   if( xSemaphoreTake( LEDMutex, ( TickType_t ) 10 ) == pdTRUE )
	   {
		   LED_Blue_On();
		   vTaskDelay(xDelay);
		   LED_Blue_Off();
		   Delay_Nonsense(&BlueDelay, &TargetCount);
		   vTaskDelay(xDelay);
		   xSemaphoreGive(LEDMutex);
	   }
   }
}

void Led_RedBlink( void *pvParameters )
{
	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
    uint32_t RedDelay = 0;
    const uint32_t TargetCount = 16000;

	for( ;; )
	{
	   for(int i = 0; i < 10; i++)
	   {
		  vTaskDelay(xDelay);
	   }

	   // Obtain the Mutex - block for 0 ticks if the semaphore is not
	   // immediately available.
	   if( xSemaphoreTake( LEDMutex, ( TickType_t ) 10 ) == pdTRUE )
	   {
		   LED_Red_On();
		   vTaskDelay(xDelay);
		   LED_Red_Off();
		   Delay_Nonsense(&RedDelay, &TargetCount);
		   vTaskDelay(xDelay);
		   xSemaphoreGive(LEDMutex);
	   }
	}
}

void Led_GreenBlink( void *pvParameters )
{
	const TickType_t xDelay = 250 / portTICK_PERIOD_MS;
    uint32_t GreenDelay = 0;
	const uint32_t TargetCount = 10000;

	for( ;; )
	{
	   for(int i = 0; i < 10; i++)
	   {
		  vTaskDelay(xDelay);
	   }

	   // Obtain the Mutex - block for 0 ticks if the semaphore is not
	   // immediately available.
	   if( xSemaphoreTake( LEDMutex, ( TickType_t ) 10 ) == pdTRUE )
	   {
		   LED_Green_On();
		   vTaskDelay(xDelay);
		   LED_Green_Off();
		   Delay_Nonsense(&GreenDelay, &TargetCount);
		   vTaskDelay(xDelay);
		   xSemaphoreGive(LEDMutex);

	   }
	}
}

/* Used to track a delay that is added to a task to make the task look
 * like it is still doing something useful */
void Delay_Nonsense(uint32_t * DelayCounter, uint32_t const * TargetCount)
{
	while(*DelayCounter <= *TargetCount)
	{
		*DelayCounter = *DelayCounter + 1;
	}

	*DelayCounter = 0;
}


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */
  xTaskCreate(Led_GreenBlink,				/* Task Pointer */
		     (const char* const)"led_green",/* Task Name */
			 configMINIMAL_STACK_SIZE,		/* Stack Depth */
			 0,								/* Parameters to pass to task*/
			 3,								/* Task Priority */
			 0);							/* Pass handle to created task */

  xTaskCreate(Led_RedBlink,					/* Task Pointer */
		     (const char* const)"led_red",  /* Task Name */
			 configMINIMAL_STACK_SIZE,		/* Stack Depth */
			 0,								/* Parameters to pass to task*/
			 2,								/* Task Priority */
			 0);							/* Pass handle to created task */

  xTaskCreate(Led_BlueBlink,				/* Task Pointer */
		     (const char* const)"led_blue", /* Task Name */
			 configMINIMAL_STACK_SIZE,      /* Stack Depth */
			 0,                             /* Parameters to pass to task*/
			 1,								/* Task Priority */
			 0);							/* Pass handle to created task */

  /* Attempt to create a semaphore. */
  LEDMutex = xSemaphoreCreateMutex();

  if( LEDMutex == NULL )
  {
	  /* There was insufficient FreeRTOS heap available for the semaphore to
	  be created successfully. */
  }
  else
  {
	  /* The semaphore can now be used. Its handle is stored in the
	  xSemahore variable.  Calling xSemaphoreTake() on the semaphore here
	  will fail until the semaphore has first been given. */
  }



  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
