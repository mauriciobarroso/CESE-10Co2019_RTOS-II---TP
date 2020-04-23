/*
 * operations.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

/*==================[inclusions]============================================*/

#include "operations.h"
#include "activeObject.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/



/*==================[internal functions declaration]=========================*/

static void vLowercaseConvert( MessageData_t *pxMessage );
static void vUppercaseConvert( MessageData_t *pxMessage );

/*==================[external functions definition]=========================*/

void vEventManager_Op ( Evento_t *evn )
{
    switch( evn->signal )
    {
		case SIG_INICIAR:
			//Ok!!
			break;
		case SIG_OK_CONVERSION_m:
			vDeleteActiveObject( ActiveObject_m );
			gpioToggle( LED2 );
			break;
		case SIG_OK_CONVERSION_M:
			vDeleteActiveObject( ActiveObject_M );
			gpioToggle( LED2 );
			break;
		default:
			//Ups!!
			break;
    }
}



void vOperationError( MessageData_t *pxMessage)
{
	strcpy( pxMessage->pucBlock, "ERROR" );
	pxMessage->ucLength = 5;
}

void vOperationSelect( MessageData_t *pxMessage )
{
	switch( pxMessage->pucBlock[ 0 ] )
	{
		case 'm':
			if ( ! bCreateActiveObject_m( pxMessage ) ){
				vOperationError( pxMessage );
			}
			break;
		case 'M':
			if ( ! bCreateActiveObject_M( pxMessage ) ){
				vOperationError( pxMessage );
			}
			break;
		default:
			vOperationError( pxMessage );
			break;
	}
}


/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
