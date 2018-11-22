/*
 * Zork.h
 *
 *  Created on: 17.11.2018
 *      Author: Pascal Steck
 */
#ifndef SOURCES_TEAM_COMMON_ZORK_H_
#define SOURCES_TEAM_COMMON_ZORK_H_

#include "Platform.h"
#include "FreeRTOS.h"
#include "task.h"
extern 	xTaskHandle ZorkTaskHndl;
extern 	xTaskHandle ShellTaskHandle;



#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
/*!
 * \brief Shell parser routine.
 * \param cmd Pointer to command line string.
 * \param handled Pointer to status if command has been handled. Set to TRUE if command was understood.
 * \param io Pointer to stdio handle
 * \return Error code, ERR_OK if everything was ok.
 */
  uint8_t ZORK_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
#endif


void ZORK_quit(void);

void ZORK_Init(void);

void ZORK_Deinit(void);


#endif /* SOURCES_TEAM_COMMON_ZORK_H_ */
