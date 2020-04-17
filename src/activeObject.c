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

QueueHandle_t xQueue;

/*==================[external data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void vThread( void *pvParameters );
static void algo( uint8_t ucActiveObjectNumber, UartDriverEvent_t *pxUartDriverEvent );

/*==================[external functions definition]=========================*/

UartPacket_t vActiveObjectEventDispatcher( UartDriverEvent_t *pxUartDriverEvent )
{
	UartPacket_t xPacket;

	xQueue = xQueueCreate( 4, sizeof( UartPacket_t ) );

	switch ( pxUartDriverEvent->EventType )
	{
		case UART_PACKET_LOWERCASE:
		{
			algo( 0, pxUartDriverEvent );

			break;
		}

		case UART_PACKET_UPPERCASE:
		{
			algo( 1, pxUartDriverEvent );

			break;
		}

		default:
			vOperationError( &pxUartDriverEvent->xPacket );
			xQueueSend( xQueue, ( void * )&pxUartDriverEvent->xPacket, 0 );

			break;
	}

	xQueueReceive( xQueue, &xPacket, portMAX_DELAY );

	vQueueDelete( xQueue );

	return xPacket;
}

bool_t bActiveObjectCreate( ActiveObjectConf_t *pxActiveObjectConf )
{
	BaseType_t xStatus;
	/* se crea la cola de recepción para el paquede del objeto activo  y retorna FALSE si no se creo correctamente*/
	pxActiveObjectConf->xQueue = xQueueCreate( LENGTH_QUEUE_AO, sizeof( ActiveObjectConf_t ) );
	if( pxActiveObjectConf->xQueue == NULL )
		return FALSE;
	/* se crea el thread en el que corre el objeto activo y retorna FALSE si no se creo correctamente */
	xStatus = xTaskCreate( vThread, "Active Object", configMINIMAL_STACK_SIZE * 2, ( void * )pxActiveObjectConf, pxActiveObjectConf->uxPriority, pxActiveObjectConf->TaskHandle_t );
	if( !xStatus == pdPASS  )
		return FALSE;
	/* retorna TRUE si se crearon correctamente la cola y el thread */
	return TRUE;
}

void vActiveObjectDelete( ActiveObjectConf_t *pxActiveObjectConf )
{
	/* se elimina la cola de recepción de paquetes del objeto activo */
	vQueueDelete( pxActiveObjectConf->xQueue );
	/* se elimina el thread del objeto activo */
	vTaskDelete( pxActiveObjectConf->TaskHandle_t );
}

/*==================[internal functions definition]==========================*/

static void vThread( void *pvParameters )
{
	ActiveObjectConf_t *xActiveObject = ( ActiveObjectConf_t * )pvParameters;
	UartDriverEvent_t xUartDriverEvent;

	for( ;; )
	{
		/* se recibe por la cola el paquete a procesar, mientras queda bloqueado indefinidamente */
		xQueueReceive( xActiveObject->xQueue, &xUartDriverEvent, portMAX_DELAY );

		/* se ejecuta la función de callback y se procesa el paquete */
		xActiveObject->xCallback( &xUartDriverEvent.xPacket );
		/* se envia a la cola de recepción del despachador de eventos el paquete procesado */
		xQueueSend( xQueue, ( void * )&xUartDriverEvent.xPacket, 0 );
		/* se comprueba si no existen paquetes para procesar en la cola */
		if( !uxQueueMessagesWaiting( xActiveObject->xQueue ) )
		{
			/* se destruye el objeto activo */
			xActiveObject->bAlive = 0;
			vActiveObjectDelete( xActiveObject );
		}
	}
}

static void algo( uint8_t ucActiveObjectNumber, UartDriverEvent_t *pxUartDriverEvent )
{
	/* se comprueba si el objeto activo ya fue creado */
	if( !xActiveObject[ ucActiveObjectNumber ].bAlive )
	{
		/* se comprueba que el objeto activo se creo correctamente y se envia el paquete a procesar */
		if( bActiveObjectCreate( &xActiveObject[ ucActiveObjectNumber ] ) )
		{
			xActiveObject[ ucActiveObjectNumber ].bAlive = 1;
			xQueueSend( xActiveObject[ ucActiveObjectNumber ].xQueue, ( void * )pxUartDriverEvent, 0 );
		}
		/* si no se pudo crear el objeto activo entonces se devuelve un mensaje de error */
		else
		{
			vOperationError( &pxUartDriverEvent->xPacket );
			xQueueSend( xQueue, ( void * )&pxUartDriverEvent->xPacket, 0 );
		}
	}
	/* si el objeto activo ya fue creado se envia el paquete a procesar */
	else
		xQueueSend( xActiveObject[ ucActiveObjectNumber ].xQueue, ( void * )pxUartDriverEvent, 0 );
}

/*==================[end of file]============================================*/
