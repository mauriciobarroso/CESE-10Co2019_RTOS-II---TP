/*
 * uartIRQ.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

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

	/* se abre sección crítica */
	taskENTER_CRITICAL();

	/* se verifica que el paquete no supere el tamaño máximo de paquetes */
	if( pxUartInstance->xRxPacket.ucLength <= PACKET_SIZE )
	{
		/* se verifica que el paquete sea correcto (caracteres de inicion y fin) */
		if( bCheckPacket( &pxUartInstance->xRxPacket ) )
		{
			/* se extrae el mensaje del paquete y se manda por la cola de recepcion */
			vExtractMessage( &pxUartInstance->xRxPacket );

			/* se verifica CRC */
			if( bCheckCrc( &pxUartInstance->xRxPacket ) )
			{
				/* se manda por la cola de recepción el paquete separado de los caracteres de inicio, fin y crc */
				xQueueSend( pxUartInstance->xQueue.xRx, ( void * )&pxUartInstance->xRxPacket, portMAX_DELAY );

				/* se pide otro bloque de memoria para el proximo mensaje a recibir */
				pxUartInstance->xRxPacket.pucBlock = ( char * )QMPool_get( &pxUartInstance->xMemoryPool.xTxPool, 0 );

				/* se verifica si se obtuvo un bloque de memoria válido */
				if( pxUartInstance->xRxPacket.pucBlock == NULL )
					uartCallbackClr( pxUartInstance->xUartConfig.xName, UART_RECEIVE );
			}
		}
	}

	pxUartInstance->xRxPacket.ucLength = 0;

	/* se cierra sección crítica */
	taskEXIT_CRITICAL();
}

void vTxTimeOutHandler( TimerHandle_t xTimer )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	UartInstance_t *pxUartInstance = ( UartInstance_t * )pvTimerGetTimerID( xTimer );

	/* se abre sección crítica */
	taskENTER_CRITICAL();

	/* se agregan caracteres de retorno de carro y fin de línea al paquete */
	uartTxWrite( pxUartInstance->xUartConfig.xName, '\r' );
	uartTxWrite( pxUartInstance->xUartConfig.xName, '\n' );

	/* se libera la memoria dinámica utilizada para el paquete */
	QMPool_put( &pxUartInstance->xMemoryPool.xTxPool, ( void * )pxUartInstance->xTxPacket.pucBlock ); // verificar parametros

	/* se pone el 0 el contador de bytes transmitidos */
	pxUartInstance->ucTxCounter = 0;

	/* se cierra sección crítica */
	taskEXIT_CRITICAL();

	/* se pregunta si la cola tiene elementos pendientes y habilita las interrupciones para transmisión */
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
			/* se asigna un callback a la interrupciñon por recepción en UART */
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
			/* se asigna un callback a la interrupciñon por transmisor libre en UART */
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
	UartInstance_t *pxUartInstance = ( UartInstance_t * )pvParameters;

	/* se abre sección crítica */
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

	/* se agrega el caracter recibido en el array de memoria dinámica */
	if( pxUartInstance->xRxPacket.ucLength < PACKET_SIZE )
		pxUartInstance->xRxPacket.pucBlock[ pxUartInstance->xRxPacket.ucLength ] = uartRxRead( pxUartInstance->xUartConfig.xName );
	else
		pxUartInstance->xRxPacket.ucLength = PACKET_SIZE + 1;
	/* se sube en 1 el tamaño del paquete recibido */
	pxUartInstance->xRxPacket.ucLength++;
	/* se inicia el timer */
	xTimerStartFromISR( pxUartInstance->xTimerTimeout.xRx, &xHigherPriorityTaskWoken );

	/* se cierra sección crítica */
	taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

	/* se solicita cambio de contexto */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void vTxIsrHandler( void *pvParameters )
{
	UBaseType_t uxSavedInterruptStatus = pdFALSE;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	UartInstance_t *pxUartInstance = ( UartInstance_t * )pvParameters;

	/* se abre sección crítica */
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR(); // se abre sección crítica

	if( !pxUartInstance->ucTxCounter )
	{
		xQueueReceiveFromISR( pxUartInstance->xQueue.xTx, &pxUartInstance->xTxPacket, &xHigherPriorityTaskWoken );

		/* se verifica que el mensaje no sea de error */
		if( pxUartInstance->xTxPacket.pucBlock[ 0 ] != 'E' )
		{
			/* se agrega crc*/
			vAddCrc( &pxUartInstance->xTxPacket );
			/* se agregan caracteres de inicio y fin */
			vAddStartAndEndCharacters( &pxUartInstance->xTxPacket );
		}

		pxUartInstance->xTxPacket.ucLength += 1;
	}
	/* se transmiten todos los bytes del bloque de transmisión */
	if( pxUartInstance->ucTxCounter < pxUartInstance->xTxPacket.ucLength - 1 )
	{
		uartTxWrite( pxUartInstance->xUartConfig.xName, pxUartInstance->xTxPacket.pucBlock[ pxUartInstance->ucTxCounter ] );
		pxUartInstance->ucTxCounter++;
	}

	if( pxUartInstance->ucTxCounter == pxUartInstance->xTxPacket.ucLength - 1 )
	{
		uartCallbackClr( pxUartInstance->xUartConfig.xName, UART_TRANSMITER_FREE );
		xTimerStartFromISR( pxUartInstance->xTimerTimeout.xTx, &xHigherPriorityTaskWoken );
	}

	/* se cierra sección crítica */
	taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

	/* se solicita cambio de contexto */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*==================[external functions definition]=========================*/

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
