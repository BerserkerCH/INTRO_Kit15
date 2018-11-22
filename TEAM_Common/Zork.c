/*
 * Zork.c
 *
 *  Created on: 17.11.2018
 *      Author: Pascal Steck
 */
#include "Platform.h"
#include "RTOS.h"
#include "Shell.h"
#include "Zork.h"

#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"

	static uint8_t ZORK_PrintHelp(const CLS1_StdIOType *io) {
	  CLS1_SendHelpStr((unsigned char*)"zork", (unsigned char*)"Group of zork commands\r\n", io->stdOut);
	  CLS1_SendHelpStr((unsigned char*)"  help", (unsigned char*)"Shows present help text\r\n", io->stdOut);
	  CLS1_SendHelpStr((unsigned char*)"  highscore", (unsigned char*)"Shows highscore\r\n", io->stdOut);
	  CLS1_SendHelpStr((unsigned char*)"  play Zork", (unsigned char*)"Starts Zork game\r\n", io->stdOut);
	  return ERR_OK;
	}

	static uint8_t ZORK_PrintHighscore(const CLS1_StdIOType *io) {
		// t.b.d
	}

	uint8_t ZORK_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
	  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"zork help")==0) {
		*handled = TRUE;
		return ZORK_PrintHelp(io);
	  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"zork highscore")==0) {
		*handled = TRUE;
		return ZORK_PrintHighscore(io);
	  }else if( UTIL1_strcmp((char*)cmd, "zork play Zork")==0){
		  vTaskResume(ZorkTaskHndl); /* run the game */
		  vTaskSuspend(ShellTaskHandle); /* suspend the shell */
		  *handled = TRUE;
		  return ERR_OK;
	  }

	  return ERR_OK;
	}
#endif

void ZORK_quit(){
  vTaskResume(ShellTaskHandle); /* resume the shell */
  vTaskSuspend(ZorkTaskHndl);
  //KIN1_SoftwareReset(); /* will perform Software RESET */
}

void ZORK_Init(){

}
void ZORK_Deinit(){
	/* nothing to do */
}
