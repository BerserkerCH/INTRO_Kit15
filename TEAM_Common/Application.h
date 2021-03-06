/**
 * \file
 * \brief Main application interface
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#ifndef SOURCES_FS2016_COMMON_APPLICATION_H_
#define SOURCES_FS2016_COMMON_APPLICATION_H_

#include "Platform.h"

#if PL_CONFIG_HAS_EVENTS
#include "Event.h"

void APP_EventHandler(EVNT_Handle event);
#endif
//void startZork(void);
void APP_Start(void);

void Task_init(void);

#if PL_CONFIG_HAS_BLINKY_TASK
static void BlinkyTask(void *pvParameters);
#endif
#if PL_CONFIG_HAS_BLINKY_TASK
static void BeepyTask(void *pvParameters);
#endif

#endif /* SOURCES_FS2016_COMMON_APPLICATION_H_ */
