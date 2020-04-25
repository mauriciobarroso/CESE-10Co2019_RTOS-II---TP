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

	/* se capturan los datos de los parametros del callback */	UartInstance_t *pxUartInstance = ( UartInstance_t * )pvTimerGetTimerID( xTimer );

	/* se abre sección crítica */
	taskENTER_CRITICAL();

	//gpioToggle( LED2 );
	gpioToggle( LED2 );
	pxUartInstance->xRxPacket.ucLength = 0;

	/* se cierra sección crítica */
	taskEXIT_CRITICAL();
}

void vTxTimeOutHandler( TimerHandle_t xTimer )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* se capturan los datos de los parametros del callback */
	UartInstance_t *pxUartInstance = ( UartInstance_t * )pvTimerGetTimerID( xTimer );

	/* se abre sección crítica */
	taskENTER_CRITICAL();

	/* se agregan caracteres de retorno de carro y fin de línea al paquete */
	uartTxWrite( pxUartInstance->xUartConfig.xName, '\r' );
	uartTxWrite( pxUartInstance->xUartConfig.xName, '\n' );

	/* se pone el 0 el contador de bytes transmitidos */
	pxUartInstance->ucTxCounter = 0;

	gpioWrite( LED1, OFF );

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

	/* se capturan los datos de los parametros del callback */
	UartInstance_t *pxUartInstance = ( UartInstance_t * )pvParameters;

	/* se abre sección crítica */
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

	/* se agrega el caracter recibido en el array de memoria dinámica */
	if( pxUartInstance->xRxPacket.ucLength < PACKET_SIZE )
	{
		pxUartInstance->xRxPacket.pucBlock[ pxUartInstance->xRxPacket.ucLength ] = uartRxRead( pxUartInstance->xUartConfig.xName );

		/* se inicia el timer */
		xTimerStartFromISR( pxUartInstance->xTimerTimeout.xRx, &xHigherPriorityTaskWoken );

		/* se sube en 1 el tamaño del paquete recibido */
		pxUartInstance->xRxPacket.ucLength++;
	}
	else
		/* si el tamaño del paquete es superior al tamaño máximo entonces se descarta el paquete */
		pxUartInstance->xRxPacket.ucLength = 0;

	/* se verifica que el primer byte es '(' o se descarta el paquete */
	if( pxUartInstance->xRxPacket.pucBlock[ 0 ] != '(' )
		pxUartInstance->xRxPacket.ucLength = 0;

	/* se verifica cuando el paquete contiene ')' y se procesa */
	if( pxUartInstance->xRxPacket.pucBlock[ pxUartInstance->xRxPacket.ucLength - 1 ] == ')' )
	{
		/* se para el timer cuando se recibe un paquete con los caracteres de inicio y fin correctos*/
		xTimerStopFromISR( pxUartInstance->xTimerTimeout.xRx, &xHigherPriorityTaskWoken );

		/* se extrae el memsaje (se elimanan '(' y ')') */
		vExtractMessage( &pxUartInstance->xRxPacket );

		/* se verifica y se elimina el CRC */
		if( bCheckCrc( &pxUartInstance->xRxPacket ) )
		{
			/* se manda por la cola de recepción el paquete separado de los caracteres de inicio, fin y crc */
			if( xQueueSendFromISR( pxUartInstance->xQueue.xRx, ( void * )&pxUartInstance->xRxPacket, &xHigherPriorityTaskWoken ) == pdTRUE )
			{
				/* se pide otro bloque de memoria para el proximo mensaje a recibir */
				pxUartInstance->xRxPacket.pucBlock = ( char * )QMPool_get( &pxUartInstance->xMemoryPool.xTxPool, 0 );

				/* se verifica si se obtuvo un bloque de memoria válido */
				if( pxUartInstance->xRxPacket.pucBlock == NULL )
					uartCallbackClr( pxUartInstance->xUartConfig.xName, UART_RECEIVE );
			}
		}
		/* se descarta el paquete si ya fue enviado o si la verificaciónde CRC falló */
		pxUartInstance->xRxPacket.ucLength = 0;
	}

	/* se cierra sección crítica */
	taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

	/* se solicita cambio de contexto */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void vTxIsrHandler( void *pvParameters )
{
	UBaseType_t uxSavedInterruptStatus = pdFALSE;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* se capturan los datos de los parametros del callback */
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

		pxUartInstance->xTxPacket.ucLength++;
	}

	/* se transmiten todos los bytes del bloque de transmisión */
	if( pxUartInstance->ucTxCounter < pxUartInstance->xTxPacket.ucLength - 1 )
	{
		uartTxWrite( pxUartInstance->xUartConfig.xName, pxUartInstance->xTxPacket.pucBlock[ pxUartInstance->ucTxCounter ] );
		pxUartInstance->ucTxCounter++;
	}

	if( pxUartInstance->ucTxCounter == pxUartInstance->xTxPacket.ucLength - 1 )
	{
		/* se libera la memoria dinámica utilizada para el paquete */
		QMPool_put( &pxUartInstance->xMemoryPool.xTxPool, ( void * )pxUartInstance->xTxPacket.pucBlock ); // verificar parametros

		/* se elimina el callback de transmisión de la UART */
		uartCallbackClr( pxUartInstance->xUartConfig.xName, UART_TRANSMITER_FREE );

		/* se inicia el timer de transmisión */
		xTimerStartFromISR( pxUartInstance->xTimerTimeout.xTx, &xHigherPriorityTaskWoken );

		gpioWrite( LED1, ON );
	}

	/* se cierra sección crítica */
	taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

	/* se solicita cambio de contexto */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*==================[external functions definition]=========================*/

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
