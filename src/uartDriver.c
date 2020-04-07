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

#include "uartDriver.h"
#include "uartIRQ.h"
#include "operations.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void vPacketTx( UartInstance_t *pxUartInstance, MessageData_t *pxMessage );
static bool_t bCheckCharacters( MessageData_t *pxMessage );

/*==================[external functions definition]=========================*/


bool_t bUartDriverInit( UartInstance_t *pxUartInstance )
{
	/* inicialización de variables */
	pxUartInstance->ucTxCounter = 0;
	pxUartInstance->xRxMessage.ucLength = 0;
	pxUartInstance->xTxMessage.ucLength = 0;
	/* configuración UART */
	uartConfig( pxUartInstance->xUartConfig.xName, pxUartInstance->xUartConfig.ulBaudRate );
	if( !bRxInterruptEnable( pxUartInstance ) )
		return FALSE;
	uartInterrupt( pxUartInstance->xUartConfig.xName, TRUE );
	/* creación de colas */
	pxUartInstance->xQueue.xRx = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( MessageData_t ) );
	if( pxUartInstance->xQueue.xRx == NULL)
		return FALSE;
	pxUartInstance->xQueue.xTx = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( MessageData_t ) );
	if( pxUartInstance->xQueue.xTx == NULL)
		return FALSE;
	/* creación de timers */
	pxUartInstance->xTimerTimeout.xRx = xTimerCreate( "Rx Timeout", PROTOCOL_TIMEOUT, pdFALSE, ( void * )pxUartInstance, vRxTimeOutHandler );
	if( pxUartInstance->xTimerTimeout.xRx == NULL)
		return FALSE;
	pxUartInstance->xTimerTimeout.xTx = xTimerCreate( "Tx Timeout", PROTOCOL_TIMEOUT, pdFALSE, ( void * )pxUartInstance, vTxTimeOutHandler );
	if( pxUartInstance->xTimerTimeout.xTx == NULL)
		return FALSE;
	/* memory pool */
	pxUartInstance->xMemoryPool.pucPoolStorage = ( char * )pvPortMalloc( POOL_SIZE * sizeof( char ) );
	if( pxUartInstance->xMemoryPool.pucPoolStorage == NULL)
		return FALSE;
	QMPool_init( &pxUartInstance->xMemoryPool.xTxPool, ( void * )pxUartInstance->xMemoryPool.pucPoolStorage, POOL_SIZE * sizeof( char ), BLOCK_SIZE );
	pxUartInstance->xRxMessage.pucBlock = ( char * )QMPool_get( &pxUartInstance->xMemoryPool.xTxPool, 0 );

	return TRUE;
}


void vUartDriverProcessPacket( UartInstance_t *pxUartInstance )
{
	MessageData_t pxMessage;
	/* se recibe el puntero del mensaje proveniente de la capa anterior */
	xQueueReceive( pxUartInstance->xQueue.xRx, &pxMessage, portMAX_DELAY );
	/* se verifica que el puntero no sea nulo y se ejecuta la operación correspondiente
	 * si el mensaje es valido o se manda un mensaje de error */
	if( &pxMessage != NULL )
	{
		if( bCheckCharacters( &pxMessage ) )
			vOperationSelect( &pxMessage );
		else
			vOperationError( &pxMessage );

		vPacketTx( pxUartInstance, &pxMessage );
	}
}

/*==================[internal functions definition]==========================*/

static void vPacketTx( UartInstance_t *pxUartInstance, MessageData_t *pxMessage )
{
	/* se envia el puntero al mensaje a la capa de separación de frames */
	xQueueSend( pxUartInstance->xQueue.xTx, ( void * )pxMessage, portMAX_DELAY );
	/* se abre sección crítica */
	taskENTER_CRITICAL();

	if( !pxUartInstance->ucTxCounter )
		bTxInterruptEnable( pxUartInstance );

	uartSetPendingInterrupt( pxUartInstance->xUartConfig.xName );
	/* se cierra sección crítica */
	taskEXIT_CRITICAL();
}

static bool_t bCheckCharacters( MessageData_t *pxMessage )
{
    for( uint8_t ucIndex = 0; ucIndex < pxMessage->ucLength; ucIndex++ )
    {
    	if( !( ( pxMessage->pucBlock[ ucIndex ] >= 'A' && pxMessage->pucBlock[ ucIndex ] <= 'Z' ) || ( pxMessage->pucBlock[ ucIndex ] >= 'a' && pxMessage->pucBlock[ ucIndex ] <= 'z' ) ) )
    		return FALSE;
    }

    return TRUE;
}

/*==================[end of file]============================================*/
