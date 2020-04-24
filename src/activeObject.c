/*
 * activeObject.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

/*==================[inclusions]============================================*/

#include "activeObject.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/



/*==================[external data declaration]==============================*/
uint8_t ucActiveObjectNumber = 0;
ActiveObject_t xActiveObject[ MAX_ACTIVE_OBJECTS_NUMBER ];


/*==================[internal functions declaration]=========================*/

static void vThread( void *pvParameters );

/*==================[external functions definition]=========================*/

bool_t bActiveObjectRegister( ActiveObjectConf_t *pxActiveObjectConf, eEventType_t EventType )
{
	if( ucActiveObjectNumber >= MAX_ACTIVE_OBJECTS_NUMBER )
		return FALSE;

	xActiveObject[ ucActiveObjectNumber ].eEventType = EventType;
	xActiveObject[ ucActiveObjectNumber ].xActiveObjectConf.bAlive = 0;
	xActiveObject[ ucActiveObjectNumber ].xActiveObjectConf.uxPriority = pxActiveObjectConf->uxPriority;
	xActiveObject[ ucActiveObjectNumber ].xActiveObjectConf.xCallback = pxActiveObjectConf->xCallback;

	ucActiveObjectNumber++;

	return TRUE;
}


bool_t bActiveObjectCreate( ActiveObjectConf_t *pxActiveObjectConf )
{
	BaseType_t xStatus;
	/* se crea la cola de recepción para el paquede del objeto activo  y retorna FALSE si no se creo correctamente*/
	pxActiveObjectConf->xQueue = xQueueCreate( LENGTH_QUEUE_AO, sizeof( ActiveObjectConf_t ) );
	if( pxActiveObjectConf->xQueue == NULL )
		return FALSE;
	/* se crea el thread en el que corre el objeto activo y retorna FALSE si no se creo correctamente */
	xStatus = xTaskCreate( vThread, "Active Object", configMINIMAL_STACK_SIZE * 2, ( void * )pxActiveObjectConf, pxActiveObjectConf->uxPriority, pxActiveObjectConf->xTask );
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
	vTaskDelete( pxActiveObjectConf->xTask );
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
		xQueueSend( xUartDriverEvent.xReceptionQueue, ( void * )&xUartDriverEvent.xPacket, 0 );

		/* se comprueba si no existen paquetes para procesar en la cola */
		if( !uxQueueMessagesWaiting( xActiveObject->xQueue ) )
		{
			/* se destruye el objeto activo */
			xActiveObject->bAlive = 0;
			vActiveObjectDelete( xActiveObject );
		}
	}
}


/*==================[end of file]============================================*/
