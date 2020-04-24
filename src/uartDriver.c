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

static QueueHandle_t xAppQueue;

/*==================[external data declaration]==============================*/

extern ActiveObject_t xActiveObject[ MAX_ACTIVE_OBJECTS_NUMBER ];

/*==================[internal functions declaration]=========================*/

static void vPacketTx( UartInstance_t *pxUartInstance, UartPacket_t *pxPacket );
static bool_t bEventBuild( UartDriverEvent_t *pxUartDriverEvent );
static void vEventDispatcher( UartDriverEvent_t *pxUartDriverEvent );

/*==================[external functions definition]=========================*/


bool_t bUartDriverInit( UartInstance_t *pxUartInstance )
{
	xAppQueue = xQueueCreate( 4, sizeof( UartPacket_t ) );

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
	/* se declara una varibale para construir el evento a enviar al AO */
	UartDriverEvent_t xUartDriverEvent;

	/* se recibe el puntero de la estructura del paquete proveniente de la capa anterior */
	xQueueReceive( pxUartInstance->xQueue.xRx, &xUartDriverEvent.xPacket, portMAX_DELAY );
	/* se verifica que el puntero no sea nulo */
	if( &xUartDriverEvent.xPacket != NULL )
	{
		/* se verifica que el paquete solo contenga caracteres alfabeticos */
		if( bCheckCharacters( &xUartDriverEvent.xPacket ) )
		{
			/* se contruye el evento (cola de recepción de respuesta del AO, tipo de evento y paquete a procesar */
			if ( bEventBuild( &xUartDriverEvent ) )
			{
				/* se envia el evento al despachador de eventos y este creará el AO correspondiente */
				vEventDispatcher( &xUartDriverEvent );

				/* se recibe la respuesta del objeto activo */
				xQueueReceive( xAppQueue, &xUartDriverEvent.xPacket, portMAX_DELAY );
			}
			else
				/* se escribe un mensaje de error si no se pudo construir el evento */
				vOperationError( &xUartDriverEvent.xPacket );
		}

		else
			/* se escribe un mensaje de error si el paquete no es correcto */
			vOperationError( &xUartDriverEvent.xPacket );

		/* se envia el paquete hacia la capa 2 */
		vPacketTx( pxUartInstance, &xUartDriverEvent.xPacket );
	}
}

/*==================[internal functions definition]==========================*/

static void vPacketTx( UartInstance_t *pxUartInstance, UartPacket_t *pxPacket )
{
	/* se envia el puntero al mensaje a la capa de separación de frames */
	xQueueSend( pxUartInstance->xQueue.xTx, ( void * )pxPacket, 0 );

	/* se abre sección crítica */
	taskENTER_CRITICAL();

	if( !pxUartInstance->ucTxCounter )
		bTxInterruptEnable( pxUartInstance );

	uartSetPendingInterrupt( pxUartInstance->xUartConfig.xName );
	/* se cierra sección crítica */
	taskEXIT_CRITICAL();
}

static bool_t bEventBuild( UartDriverEvent_t *pxUartDriverEvent )
{
	/* se construye el evento con la cola de recpción de resultados, bloque de memoria del paque y la longitud del mismo */
	pxUartDriverEvent->xReceptionQueue = xAppQueue;

	switch( pxUartDriverEvent->xPacket.pucBlock[ 0 ] )
	{
		case 'm':
			pxUartDriverEvent->EventType = UART_PACKET_LOWERCASE;
			break;
		case 'M':
			pxUartDriverEvent->EventType = UART_PACKET_UPPERCASE;
			break;
		/*case 'A':
			pxUartDriverEvent->EventType = UART_PACKET_UPPERLOWERCASE;
			break;*/
		default:
			return FALSE;
	}

	return TRUE;
}

static void vEventDispatcher( UartDriverEvent_t *pxUartDriverEvent )
{
	/* se crean los AOs que tengan registrado el evento actual */
	for( uint8_t i = 0; i < MAX_ACTIVE_OBJECTS_NUMBER; i++ )
	{
		if( xActiveObject[ i ].eEventType == pxUartDriverEvent->EventType )
		{
			/* se comprueba si el objeto activo ya fue creado */
			if( !xActiveObject[ i ].xActiveObjectConf.bAlive )
			{
				/* se comprueba que el objeto activo se creo correctamente y se envia el paquete a procesar a la cola de este */
				if( bActiveObjectCreate( &xActiveObject[ i ].xActiveObjectConf ) )
				{
					xActiveObject[ i ].xActiveObjectConf.bAlive = 1;
					xQueueSend( xActiveObject[ i ].xActiveObjectConf.xQueue, ( void * )pxUartDriverEvent, 0 );
				}

				/* si no se pudo crear el objeto activo entonces se devuelve un mensaje de error a C3 */
				else
				{
					vOperationError( &pxUartDriverEvent->xPacket );
					xQueueSend( pxUartDriverEvent->xReceptionQueue, ( void * )&pxUartDriverEvent->xPacket, 0 );
				}
			}
			/* si el objeto activo ya fue creado se envia el paquete a procesar a este */
			else
				xQueueSend( xActiveObject[ i ].xActiveObjectConf.xQueue, ( void * )pxUartDriverEvent, 0 );
		}
	}
}

/*==================[end of file]============================================*/
