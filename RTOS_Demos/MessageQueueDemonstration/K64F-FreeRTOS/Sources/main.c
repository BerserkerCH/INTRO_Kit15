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
#include "queue.h"
#include <stdbool.h>

typedef enum
{
	LED_BLUE,
	LED_RED,
	LED_GREEN,
	LED_MAX_NUMBER
}LED_t;

typedef enum
{
    LED_ON,
	LED_OFF,
	LED_STATE_MAX
}LEDState_t;

typedef struct
{
    LED_t LED;
    LEDState_t State;
    uint32_t TimeDelay;
}LedMessage_t;

QueueHandle_t LedQueue;

void Led_GateKeeper(void *pvParameters)
{
   LedMessage_t * LedMessagePtr;

   TickType_t xDelay = 500 / portTICK_PERIOD_MS;

   for(;;)
   {

	   // Obtain the Mutex - block for 0 ticks if the semaphore is not
	   // immediately available.
	   if(xQueueReceive(LedQueue, &(LedMessagePtr),(TickType_t)10) )
	   {
		   if(LedMessagePtr->LED == LED_BLUE)
		   {
			   if(LedMessagePtr->State == LED_ON)
			   {
				   LED_Blue_On();
			   }
			   else
			   {
				   LED_Blue_Off();
			   }
		   }
		   else if(LedMessagePtr->LED == LED_RED)
		   {
			   if(LedMessagePtr->State == LED_ON)
			   {
				   LED_Red_On();
			   }
			   else
			   {
				   LED_Red_Off();
			   }

		   }
		   else if(LedMessagePtr->LED == LED_GREEN)
		   {
			   if(LedMessagePtr->State == LED_ON)
			   {
				   LED_Green_On();
			   }
			   else
			   {
				   LED_Green_Off();
			   }
		   }
		   else
		   {
			   // Received an unsupported LED color
		   }

		   xDelay = LedMessagePtr->TimeDelay / portTICK_PERIOD_MS;
		   vTaskDelay(xDelay);
	   }
   }
}

void Led_Controller(void *pvParameters)
{
   LedMessage_t LedMessage;
   LedMessage_t * LedMessagePtr = &LedMessage;
   TickType_t xDelay = 500 / portTICK_PERIOD_MS;

   for(;;)
   {
	   for(LED_t Led = LED_BLUE; Led < LED_MAX_NUMBER; Led++)
	   {
		   for(LEDState_t State = LED_ON; State < LED_STATE_MAX; State++)
		   {
			   LedMessage.LED = Led;
			   LedMessage.State = State;
			   LedMessage.TimeDelay = 250;

			   xQueueSend(LedQueue, (void *)&LedMessagePtr, (TickType_t)0);

			   xDelay = LedMessage.TimeDelay / portTICK_PERIOD_MS;
			   vTaskDelay(xDelay);
		   }
	   }
   }
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
  xTaskCreate(Led_GateKeeper,				/* Task Pointer */
		     (const char* const)"led_gate", /* Task Name */
			 configMINIMAL_STACK_SIZE,		/* Stack Depth */
			 0,								/* Parameters to pass to task*/
			 2,								/* Task Priority */
			 0);							/* Pass handle to created task */

  xTaskCreate(Led_Controller,		    	/* Task Pointer */
		     (const char* const)"led_ctrl", /* Task Name */
			 configMINIMAL_STACK_SIZE,		/* Stack Depth */
			 0,								/* Parameters to pass to task*/
			 3,								/* Task Priority */
			 0);							/* Pass handle to created task */

  LedQueue = xQueueCreate( 10, sizeof( LedMessage_t * ) );

  if( LedQueue == 0 )
  {
	  // Failed to create the queue.
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
