/* Copyright 2020, Mauricio Barroso
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* Date: 19/03/20 */

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
#define MAX_ACTIVE_OBJECTS_NUMBER	2

/*==================[typedef]================================================*/

typedef enum
{
	UART_PACKET_LOWERCASE,
	UART_PACKET_UPPERCASE,
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

/*==================[external data declaration]==============================*/

ActiveObjectConf_t xActiveObject[ MAX_ACTIVE_OBJECTS_NUMBER ];

/*==================[external functions declaration]=========================*/

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
