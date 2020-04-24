/*
 * activeObject.h
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

#ifndef _ACTIVEOBJECT_H_
#define _ACTIVEOBJECT_H_

/*==================[inclusions]=============================================*/

#include <stdint.h>
#include "string.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "uartDriver.h"
#include "operations.h"
#include "events.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define MAX_ACTIVE_OBJECTS_NUMBER	5
#define LENGTH_QUEUE_AO				10

/*==================[typedef]================================================*/

typedef struct
{
	bool_t bAlive;
	QueueHandle_t xQueue;
	TaskFunction_t xCallback;
	UBaseType_t uxPriority;
	TaskHandle_t xTask;
} ActiveObjectConf_t;

typedef struct
{
	eEventType_t eEventType;
	ActiveObjectConf_t xActiveObjectConf;
} ActiveObject_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief Function to register AOs in a AOs buffer
 *
 * @param pxActiveObjectConf[in] pointer to AO configuration
 * @param EventType[in] Event type of AO
 *
 * @return
 * 		- TRUE Successful
 * 		- FALSE Number of created AOs out of range
 */
bool_t bActiveObjectRegister( ActiveObjectConf_t *pxActiveObjectConf, eEventType_t EventType );

/**
 * @brief Function to create AO (thread and queue)
 *
 * @param pxActiveObjectConf[in] pointer to AO configuration
 *
 * @return
 * 		- TRUE Successful
 * 		- FALSE Fail to create AO (thread or queue)
 */
bool_t bActiveObjectCreate( ActiveObjectConf_t *pxActiveObjectConf );

/**
 * @brief Function to delete AO (thread and queue)
 *
 * @param pxActiveObjectConf[in] pointer to AO configuration
 *
 */
void vActiveObjectDelete( ActiveObjectConf_t *pxActiveObjectConf );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _ACTIVEOBJECT_H_ */
