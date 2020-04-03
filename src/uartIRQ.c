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

/*==================[inlcusions]============================================*/

#include "uartIRQ.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

static void vRxIsrHandler( void *pvParameters );
static void vTxIsrHandler( void *pvParameters );

/*==================[external data declaration]==============================*/

void vRxTimeOutHandler( TimerHandle_t xTimer )
{
	UartInstance_t *pxUartInstance;

	pxUartInstance = ( UartInstance_t * )pvTimerGetTimerID( xTimer );

	taskENTER_CRITICAL();

	if( pxUartInstance->xPacketLength.ucRx <= PACKET_SIZE )
	{
		/* se guarda el valor de la longitud del paquete en el primer elemento del string y se pone a cero la variable */
		pxUartInstance->xBlockPointer.pucRx[ 0 ] = pxUartInstance->xPacketLength.ucRx;
		pxUartInstance->xPacketLength.ucRx = 0;
		/* se verifica que el paquete sea correcto */
		if( bCheckPacket( pxUartInstance->xBlockPointer.pucRx ) )
		{
			/* se extrae el mensaje del paquete y se manda por la cola de recepcion*/
			vExtractMessage( pxUartInstance->xBlockPointer.pucRx );
			xQueueSend( pxUartInstance->xQueue.xRx, &pxUartInstance->xBlockPointer.pucRx, portMAX_DELAY );
			/* se pide otro bloque de memoria para el proximo mensaje a recibir */
			pxUartInstance->xBlockPointer.pucRx = ( char * )QMPool_get( &pxUartInstance->xMemoryPool.xTxPool, 0 );
			/* se verifica si se obtuvo un bloque de memoria válido */
			if( pxUartInstance->xBlockPointer.pucRx == NULL )
				uartCallbackClr( pxUartInstance->xUartConfig.xName, UART_RECEIVE );
		}
	}

	taskEXIT_CRITICAL();
}

void vTxTimeOutHandler( TimerHandle_t xTimer )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	UartInstance_t *pxUartInstance = ( UartInstance_t * )pvTimerGetTimerID( xTimer );

	taskENTER_CRITICAL();

	uartTxWrite( pxUartInstance->xUartConfig.xName, '\r' );
	//uartTxWrite( pxUartInstance->xUartConfig.xName, '\n' );

	QMPool_put( &pxUartInstance->xMemoryPool.xTxPool, ( void * )pxUartInstance->xBlockPointer.pucTx ); // verificar parametros

	pxUartInstance->ucTxCounter = 0;

	taskEXIT_CRITICAL();

	if( uxQueueMessagesWaiting( pxUartInstance->xQueue.xTx ) )
	{
		bTxInterruptEnable( pxUartInstance );
		uartSetPendingInterrupt( pxUartInstance->xUartConfig.xName );
	}
}

bool_t bRxInterruptEnable( UartInstance_t *pxUartInstance )
{
	switch( pxUartInstance->xUartConfig.xName )
	{
		case UART_GPIO:
			break;
		case UART_232:
			break;
		case UART_USB:
			uartCallbackSet( pxUartInstance->xUartConfig.xName, UART_RECEIVE, vRxIsrHandler,( void * )pxUartInstance );
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

bool_t bTxInterruptEnable( UartInstance_t *pxUartInstance )
{
	switch( pxUartInstance->xUartConfig.xName )
	{
		case UART_GPIO:
			break;
		case UART_232:
			break;
		case UART_USB:
			uartCallbackSet( pxUartInstance->xUartConfig.xName, UART_TRANSMITER_FREE, vTxIsrHandler,( void * )pxUartInstance );
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

/*==================[internal functions declaration]=========================*/

static void vRxIsrHandler( void *pvParameters )
{
	UBaseType_t uxSavedInterruptStatus = pdFALSE;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	UartInstance_t *pxUartInstance = &xUartInstance;

	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR(); // se abre sección crítica
	/* se agrega el caracter recibido en el array dinamico */
	if( pxUartInstance->xPacketLength.ucRx < PACKET_SIZE )
		pxUartInstance->xBlockPointer.pucRx[ pxUartInstance->xPacketLength.ucRx + 1 ] = uartRxRead( pxUartInstance->xUartConfig.xName );
	else
		pxUartInstance->xPacketLength.ucRx = PACKET_SIZE + 1;
	/* se sube en 1 el tamaño del paquete recibido */
	pxUartInstance->xPacketLength.ucRx++;
	/* se inicia el timer */
	xTimerStartFromISR( pxUartInstance->xTimerTimeout.xRx, &xHigherPriorityTaskWoken );

	taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void vTxIsrHandler( void *pvParameters )
{
	UBaseType_t uxSavedInterruptStatus = pdFALSE;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	UartInstance_t *pxUartInstance = &xUartInstance;

	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR(); // se abre sección crítica

	if( !pxUartInstance->ucTxCounter )
	{
		xQueueReceiveFromISR( pxUartInstance->xQueue.xTx, &pxUartInstance->xBlockPointer.pucTx, &xHigherPriorityTaskWoken );
		/* se verifica que el mensaje no sea de error */
		if( pxUartInstance->xBlockPointer.pucTx[ 1 ] != 'E' )
		{
			/* se agregan caracteres de inicio y fin */
			vAddStartAndEndCharacters( pxUartInstance->xBlockPointer.pucTx );
			/* se agrega crc*/
			vAddCrc( pxUartInstance->xBlockPointer.pucTx );
		}

		pxUartInstance->xPacketLength.ucTx = pxUartInstance->xBlockPointer.pucTx[ 0 ] + 1;

		pxUartInstance->ucTxCounter++;
	}
		/* se transmiten todos los bytes del bloque de transmisión */
	if( pxUartInstance->ucTxCounter < pxUartInstance->xPacketLength.ucTx )
	{
		uartTxWrite( pxUartInstance->xUartConfig.xName, pxUartInstance->xBlockPointer.pucTx[ pxUartInstance->ucTxCounter ] );
		pxUartInstance->ucTxCounter++;
	}

	if( pxUartInstance->ucTxCounter == pxUartInstance->xPacketLength.ucTx )
	{
		uartCallbackClr( pxUartInstance->xUartConfig.xName, UART_TRANSMITER_FREE );
		xTimerStartFromISR( pxUartInstance->xTimerTimeout.xTx, &xHigherPriorityTaskWoken );
	}

	taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*==================[external functions definition]=========================*/

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/