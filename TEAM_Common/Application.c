/**
f * \file
 * \brief Main application file
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Application.h"
#include "Event.h"
#include "LED.h"
#include "WAIT1.h"
#include "CS1.h"
#include "KeyDebounce.h"
#include "CLS1.h"
#include "KIN1.h"
#include "Drive.h"
#include "Pid.h"
#if PL_CONFIG_HAS_KEYS
  #include "Keys.h"
#endif
#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
  #include "Shell.h"
  #include "RTT1.h"
#endif
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
  #include "Q4CLeft.h"
  #include "Q4CRight.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_BOARD_IS_ROBO_V2
  #include "PORT_PDD.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
  #include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_LCD_MENU
  #include "LCD.h"
#endif
#if PL_CONFIG_HAS_SNAKE_GAME
  #include "Snake.h"
#endif
#if PL_CONFIG_HAS_REFLECTANCE
  #include "Reflectance.h"
#endif
#if PL_CONFIG_HAS_TACHO_TASK
	#include "Tacho.h"
#endif

#include "Sumo.h"

#if PL_CONFIG_HAS_EVENTS

static void BtnMsg(int btn, const char *msg) {
#if PL_CONFIG_HAS_SHELL
  #if PL_CONFIG_HAS_SHELL_QUEUE
    uint8_t buf[48];

    UTIL1_strcpy(buf, sizeof(buf), "Button pressed: ");
    UTIL1_strcat(buf, sizeof(buf), msg);
    UTIL1_strcat(buf, sizeof(buf), ": ");
    UTIL1_strcatNum32s(buf, sizeof(buf), btn);
    UTIL1_strcat(buf, sizeof(buf), "\r\n");
    SHELL_SendString(buf);
  #else
    CLS1_SendStr("Button pressed: ", CLS1_GetStdio()->stdOut);
    CLS1_SendStr(msg, CLS1_GetStdio()->stdOut);
    CLS1_SendStr(": ", CLS1_GetStdio()->stdOut);
    CLS1_SendNum32s(btn, CLS1_GetStdio()->stdOut);
    CLS1_SendStr("\r\n", CLS1_GetStdio()->stdOut);
  #endif
#endif
}

void APP_EventHandler(EVNT_Handle event) {
  /*! \todo handle events */
  switch(event) {
  case EVNT_STARTUP:
    {
      int i;
      for (i=0;i<5;i++) {
        LED1_Neg();
        WAIT1_Waitms(50);
      }
      LED1_Off();
    }
    break;
  case EVNT_LED_HEARTBEAT:
    LED1_Neg();

    break;
  case EVNT_BUZZ_NEG:
 //   BUZ_PlayTune(BUZ_TUNE_WELCOME); // does not work yet
    break;
#if PL_CONFIG_NOF_KEYS>=1
  case EVNT_SW1_PRESSED:
	  BtnMsg(1,NULL);
	  PID_Init(PID_CONFIG_AGRO);
	  LF_StartFollowing();
 //   BUZ_Beep(100,100);
    LED1_On();

    break;
  case   EVNT_SW1_LPRESSED:
 //    BUZ_Beep(600,300);
     LED1_On();
     //CLS1_SendStr("SW1 long pressed!\r\n", CLS1_GetStdio()->stdOut);
     break;
  case   EVNT_SW1_RELEASED:
//	 BUZ_Beep(60,200);
     LED1_Off();
     CLS1_SendStr("SW1 released!\r\n", CLS1_GetStdio()->stdOut);
     break;
#endif
    default:
      break;
   } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */

#if PL_CONFIG_HAS_MOTOR /* currently only used for robots */
static const KIN1_UID RoboIDs[] = {
  /* 0: L20, V2 */ {{0x00,0x03,0x00,0x00,0x67,0xCD,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 1: L21, V2 */ {{0x00,0x05,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 2: L4, V1  */ {{0x00,0x0B,0xFF,0xFF,0x4E,0x45,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}},
  /* 3: L23, V2 */ {{0x00,0x0A,0x00,0x00,0x67,0xCD,0xB8,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 4: L11, V2 */ {{0x00,0x19,0x00,0x00,0x67,0xCD,0xB9,0x11,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 5: L5, V2 */  {{0x00,0x38,0x00,0x00,0x67,0xCD,0xB5,0x41,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 6: L3, V1 */  {{0x00,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x0A}},
  /* 7: L1, V1 */  {{0x00,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x25}},
};
#endif

static void APP_AdoptToHardware(void) {
  KIN1_UID id;
  uint8_t res;

  res = KIN1_UIDGet(&id);
  if (res!=ERR_OK) {
    for(;;); /* error */
  }

#if PL_CONFIG_HAS_MOTOR
  if (KIN1_UIDSame(&id, &RoboIDs[0])) { /* L20 */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CRight_SwapPins(TRUE);
#endif
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* invert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[1])) { /* V2 L21 */
    /* no change needed */
  } else if (KIN1_UIDSame(&id, &RoboIDs[2])) { /* V1 L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[3])) { /* L23 */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CRight_SwapPins(TRUE);
#endif
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* invert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[4])) { /* L11 */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CRight_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[5])) { /* L5, V2 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* invert right motor */
    (void)Q4CRight_SwapPins(TRUE);
  } else if (KIN1_UIDSame(&id, &RoboIDs[6])) { /* L3, V1 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert right motor */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[7])) { /* L1, V1 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert right motor */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
#endif
  }
#endif
#if PL_CONFIG_HAS_QUADRATURE && PL_CONFIG_BOARD_IS_ROBO_V2
  /* pull-ups for Quadrature Encoder Pins */
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 10, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 10, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 11, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 11, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 16, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 16, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 17, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 17, PORT_PDD_PULL_ENABLE);
#endif
}

/* void myISR(void){
	EVNT_SetEvent(EVNT_LED_HEARTBEAT);
} */
#if PL_CONFIG_HAS_BLINKY_TASK
static void BlinkyTask(void *pvParameters){
	SHELL_SendString("Blinky started.\r\n");

	for(;;){
		LED1_Neg();
		EVNT_HandleEvent(APP_EventHandler,TRUE);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
#endif
#if PL_CONFIG_HAS_BEEP_TASK
static void BeepyTask(void *pvParameters){
	SHELL_SendString("I am Blinky\r\n");
	for(;;){
	    BUZ_Beep(100,2);
	    vTaskDelay(pdMS_TO_TICKS(100));
	}
}
#endif
#if PL_CONFIG_HAS_TACHO_TASK
static void TachoTask(void *pvParameters){
	for(;;){
		TACHO_Sample();
		vTaskDelay(pdMS_TO_TICKS(5));
	}
}
#endif

void Task_init(void){
#if PL_CONFIG_HAS_BLINKY_TASK
	 BaseType_t blink_res;
		  blink_res = xTaskCreate(BlinkyTask, "Blinky", 500/sizeof(StackType_t), (void*)NULL, tskIDLE_PRIORITY+1, NULL);
		  if (blink_res != pdPASS){
				  // something went wrong
				 for(;;);
		  }
#endif
#if PL_CONFIG_HAS_BEEP_TASK
		  BaseType_t beep_res;
		  beep_res = xTaskCreate(BeepyTask, "Beepy", 500/sizeof(StackType_t), (void*)NULL,	tskIDLE_PRIORITY+1, NULL);
		  if (beep_res != pdPASS){
				  // something went wrong
				  WAIT1_Waitms(10);
		  }
#endif
#if PL_CONFIG_HAS_TACHO_TASK
		  BaseType_t tacho_res;
		  tacho_res = xTaskCreate(TachoTask, "Tacho", 500/sizeof(StackType_t), (void*)NULL,	tskIDLE_PRIORITY+2, NULL);
		  if (tacho_res != pdPASS){
				  // something went wrong
				  for(;;);
		  }
#endif
}

void APP_Start(void) {
  PL_Init();
  APP_AdoptToHardware();
  //__asm volatile("cpsie i"); /* enable interrupts */
  EVNT_SetEvent(EVNT_STARTUP);

  //PID_LoadSettingsFromFlash();
  DRV_SetMode(DRV_MODE_POS);


  Task_init();
  vTaskStartScheduler() ;
  for(;;) {
	  EVNT_HandleEvent(APP_EventHandler,TRUE);
  }
}


