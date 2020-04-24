/*
 * activeObject.h
 *
 *  Created on: Apr 9, 2020
 *      Author: pablo
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

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define LENGTH_QUEUE_AO				10
#define MAX_ACTIVE_OBJECTS_NUMBER	5

/*==================[typedef]================================================*/

typedef enum
{
	UNKNOW,
	UART_PACKET_LOWERCASE,
	UART_PACKET_UPPERCASE,
	UART_PACKET_UPPERLOWERCASE,
	UART_PACKET_ERROR
} eEventType_t;

typedef struct
{
	eEventType_t EventType;
	UartPacket_t xPacket;
} UartDriverEvent_t;

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

bool_t bActiveObjectRegister( eEventType_t eEventType, ActiveObjectConf_t *pxActiveObjectConf );
UartPacket_t vActiveObjectEventDispatcher( UartDriverEvent_t *pxUartDriverEvent );
bool_t bActiveObjectCreate( ActiveObjectConf_t *xActiveObjectConf );
void vActiveObjectDelete( ActiveObjectConf_t *pxActiveObjectConf );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _ACTIVEOBJECT_H_ */
