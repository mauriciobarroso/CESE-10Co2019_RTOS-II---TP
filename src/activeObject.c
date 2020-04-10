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

/*==================[inclusions]============================================*/

#include "activeObject.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void vThread1( void *pvParameters );
static void vThread2( void *pvParameters );

/*==================[external functions definition]=========================*/

void vSendToActiveObject( UartDriverEvent_t *pxUartDriverEvent, ActiveObjectConf_t *xActiveObject )
{
	uint8_t ucOperation;

	ucOperation = pxUartDriverEvent->xMessage.pucBlock[ 0 ];

	switch( ucOperation )
	{
		case 'm':
		{
			// primero preguntar si existe el AO

			xActiveObject[ 0 ].xTaskFuncion = vThread1;
			xActiveObject[ 0 ].xQueue = xQueueCreate( LENGTH_QUEUE_AO, sizeof( UartDriverEvent_t ) );
			if( xActiveObject[ 0 ].xQueue == NULL )
				break;
			/* se crea el objeto activo */
			xCreateActiveObject( &xActiveObject[ 0 ] );
			/* se envia el evento a la cola del objeto activo creado */
			xQueueSend( xActiveObject[ 0 ].xQueue, ( void * )pxUartDriverEvent, 0 ); // se enviar el evento al AO creado

			break;
		}

		case 'M':
		{
			// primero preguntar si existe el AO

			xActiveObject[ 1 ].xTaskFuncion = vThread2;
			xActiveObject[ 1 ].xQueue = xQueueCreate( LENGTH_QUEUE_AO, sizeof( UartDriverEvent_t ) );
			if( xActiveObject[ 1 ].xQueue == NULL )
				break;
			/* se crea el objeto activo */
			xCreateActiveObject( &xActiveObject[ 1 ] );
			/* se envia el evento a la cola del objeto activo creado */
			xQueueSend( xActiveObject[ 1 ].xQueue, ( void * )pxUartDriverEvent, 0 ); // se enviar el evento al AO creado
			break;
		}

		default:
			break;
	}
}

void xCreateActiveObject( ActiveObjectConf_t *xActiveObjectConf )
{
	xTaskCreate( xActiveObjectConf->xTaskFuncion, "Active Object", configMINIMAL_STACK_SIZE * 2, ( void * )xActiveObjectConf->xQueue, tskIDLE_PRIORITY + 3, NULL );
}

void vDeleteActiveObject( TaskHandle_t xTask, QueueHandle_t xQueue )
{
	vQueueDelete( xQueue );
	vTaskDelete( xTask );
}

/*==================[internal functions definition]==========================*/

static void vThread1( void *pvParameters )
{
	QueueHandle_t xQueue = ( QueueHandle_t )pvParameters ;
	UartDriverEvent_t xUartDriverEvent;

	for( ;; )
	{
		xQueueReceive( xQueue, &xUartDriverEvent, portMAX_DELAY );

		/* event dispatcher */
		switch( xUartDriverEvent.EventType )
		{
			case UART_PACKET_MM:
				/* se abre sección crítica para que la tarea se ejecute sin interupciones */
				taskENTER_CRITICAL();
				vLowercaseConvert( &xUartDriverEvent.xMessage );
				taskEXIT_CRITICAL();
				break;

			default:
				break;
		}
		/* se elimina el objeto activo si no existen mas mensajes en la cola */
		if( !uxQueueMessagesWaiting( xQueue ) )
			vDeleteActiveObject( NULL, xQueue );
	}
}

static void vThread2( void *pvParameters )
{
	QueueHandle_t xQueue = ( QueueHandle_t )pvParameters ;
	UartDriverEvent_t xUartDriverEvent;

	for( ;; )
	{
		xQueueReceive( xQueue, &xUartDriverEvent, portMAX_DELAY );

		/* event dispatcher */
		switch( xUartDriverEvent.EventType )
		{
			case UART_PACKET_MM:
				/* se abre sección crítica para que la tarea se ejecute sin interupciones */
				taskENTER_CRITICAL();
				vUppercaseConvert( &xUartDriverEvent.xMessage );
				taskEXIT_CRITICAL();
				break;

			default:
				break;
		}
			/* se elimina el objeto activo si no existen mas mensajes en la cola */
		if( !uxQueueMessagesWaiting( xQueue ) )
			vDeleteActiveObject( NULL, xQueue );
	}
}

/*==================[end of file]============================================*/
