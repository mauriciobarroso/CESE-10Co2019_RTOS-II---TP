/*
 * uartDriver.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

/*==================[inlcusions]============================================*/

#include "uartDriver.h"
#include "uartIRQ.h"
#include "operations.h"
#include "activeObject.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void vPacketTx( UartInstance_t *pxUartInstance, UartPacket_t *pxPacket );

/*==================[external functions definition]=========================*/


bool_t bUartDriverInit( UartInstance_t *pxUartInstance )
{
	/* inicialización de variables */
	pxUartInstance->ucTxCounter = 0;
	pxUartInstance->xRxPacket.ucLength = 0;
	pxUartInstance->xTxPacket.ucLength = 0;

	/* configuración UART */
	uartConfig( pxUartInstance->xUartConfig.xName, pxUartInstance->xUartConfig.ulBaudRate );
	if( !bRxInterruptEnable( pxUartInstance ) )
		return FALSE;
	uartInterrupt( pxUartInstance->xUartConfig.xName, TRUE );

	/* creación de colas */
	pxUartInstance->xQueue.xRx = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( UartPacket_t ) );
	if( pxUartInstance->xQueue.xRx == NULL)
		return FALSE;
	pxUartInstance->xQueue.xTx = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( UartPacket_t ) );
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
	pxUartInstance->xRxPacket.pucBlock = ( char * )QMPool_get( &pxUartInstance->xMemoryPool.xTxPool, 0 );

	return TRUE;
}


void vUartDriverProcessPacket( UartInstance_t *pxUartInstance )
{
	UartDriverEvent_t pxUartDriverEvent;
	UartPacket_t pxPacket;

	/* se recibe el puntero de la estructura del paquete proveniente de la capa anterior */
	xQueueReceive( pxUartInstance->xQueue.xRx, &pxPacket, portMAX_DELAY );
	/* se verifica que el puntero no sea nulo */
	if( &pxPacket != NULL )
	{
		/* se verifica que el paquete solo contenga caracteres alfabeticos */
		if( bCheckCharacters( &pxPacket ) )
		{
			/* se construye el evento con el tipo, bloque de memoria del paquete y la longitud del mismo */
			switch( pxPacket.pucBlock[ 0 ] )
			{
				case 'm':
					pxUartDriverEvent.EventType = UART_PACKET_LOWERCASE;
					break;
				case 'M':
					pxUartDriverEvent.EventType = UART_PACKET_UPPERCASE;
					break;
				case 'A':
					pxUartDriverEvent.EventType = UART_PACKET_UPPERLOWERCASE;
					break;
				default:
					pxUartDriverEvent.EventType = UART_PACKET_ERROR;
					break;
			}

			pxUartDriverEvent.xPacket = pxPacket;

			/* se envia el evento al despachador de eventos y se espera que devuelva el paquete procesado */
			pxPacket = vActiveObjectEventDispatcher( &pxUartDriverEvent );
		}

		else
			/* se escribe un mensaje de error si el paquete no es correcto */
			vOperationError( &pxPacket );

		/* se envia el paquete hacia la capa 2 */
		vPacketTx( pxUartInstance, &pxPacket );
	}
}

/*==================[internal functions definition]==========================*/

static void vPacketTx( UartInstance_t *pxUartInstance, UartPacket_t *pxPacket )
{
	/* se envia el puntero al mensaje a la capa de separación de frames */
	xQueueSend( pxUartInstance->xQueue.xTx, ( void * )pxPacket, portMAX_DELAY );
	/* se abre sección crítica */
	taskENTER_CRITICAL();

	if( !pxUartInstance->ucTxCounter )
		bTxInterruptEnable( pxUartInstance );

	uartSetPendingInterrupt( pxUartInstance->xUartConfig.xName );
	/* se cierra sección crítica */
	taskEXIT_CRITICAL();
}

/*==================[end of file]============================================*/
