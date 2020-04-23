/*
 * activeObject.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

/*==================[inclusions]============================================*/

#include "activeObject.h"
#include "sapi.h"

/*==================[macros]=================================================*/

#define	MAX_AO	5

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

static ActiveObject_t 	ActiveObjects[MAX_AO];
static int 		    	lastAO = 0;
xQueueHandle queueEvents;

/*==================[external data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void vLowercaseConvert_mM( char * , int );
static void vUppercaseConvert_mM( char * , int );

/*==================[external functions definition]=========================*/

bool_t bCreateActiveObject_m( MessageData_t * pxMessage)
{
	ActiveObject_m  = xRegistActiveObject( vEventManager_m );

	if (xTaskCreate( vTaskActiveObject_m, "vTaskActiveObject_m", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 4, ActiveObject_m->TH ))
	{
		vPutQueueEvent( ActiveObject_m, SIG_CONVERTIR_LOWERCASE, pxMessage->pucBlock, pxMessage->ucLength );
		return TRUE;
	}
	return FALSE;
}

bool_t bCreateActiveObject_M( MessageData_t * pxMessage)
{
	ActiveObject_M  = xRegistActiveObject( vEventManager_M  );

	if (xTaskCreate( vTaskActiveObject_M, "vTaskActiveObject_M", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 4, ActiveObject_M->TH ));
	{
		vPutQueueEvent( ActiveObject_M, SIG_CONVERTIR_UPPERCASE, pxMessage->pucBlock, pxMessage->ucLength );
		return TRUE;
	}
	return FALSE;
}


void vDeleteActiveObject( ActiveObject_t * AO )
{
	TaskHandle_t miTH;
	miTH = AO->TH;
	vUnRegistActiveObject( AO );
	vTaskDelete( miTH );
}



void vTaskActiveObject_m ( void * noUsed )
{
// Manejo de eventos:
// Esta siempre bloqueada salvo que tenga un evento que manejar.
// En cuanto hay un evento, lo despacho a la FSM correspondiente SIN BLOQUEAR

    Evento_t evn;
    for( ;; )
    {
    	xQueueReceive( queueEvents, &evn, portMAX_DELAY );
    	vDispatchEvent( &evn );
    }
}

void vTaskActiveObject_M ( void * noUsed )
{
// Manejo de eventos:
// Esta siempre bloqueada salvo que tenga un evento que manejar.
// En cuanto hay un evento, lo despacho a la FSM correspondiente SIN BLOQUEAR

    Evento_t evn;
    for( ;; )
    {
    	xQueueReceive( queueEvents, &evn, portMAX_DELAY );
    	vDispatchEvent( &evn );
    }
}


void vEventManager_m ( Evento_t *evn )
{
	switch( evn->signal )
	{
		case SIG_INICIAR:
			//Ok!!
			break;
		case SIG_CONVERTIR_LOWERCASE:
			vLowercaseConvert_mM( evn->pChar, evn->largo );
			vPutQueueEvent( Modulo_Op, SIG_OK_CONVERSION_m, evn->pChar, evn->largo );
			break;
		default:
			//Ups!!
			break;
	}
}

void vEventManager_M ( Evento_t *evn )
{
    switch( evn->signal )
    {
		case SIG_INICIAR:
			//Ok!!
			break;
		case SIG_CONVERTIR_UPPERCASE:
			vUppercaseConvert_mM( evn->pChar, evn->largo );
			vPutQueueEvent( Modulo_Op, SIG_OK_CONVERSION_M, evn->pChar, evn->largo );
			break;
		default:
			//Ups!!
			break;
    }
}


ActiveObject_t * xRegistActiveObject ( fsm_ptr manejadorEventos )
{
    if( lastAO >= MAX_AO )
    {
    	perror("Error: Superado maximo numero de Activ Objects");
        return NULL;
    }

    ActiveObject_t * pAO 	= &ActiveObjects[ lastAO ];
    pAO->manejadorEventos	= manejadorEventos;
    pAO->TH 				= NULL; //TaskHandle
    pAO->param_aux_1        = 0;
    pAO->param_aux_2 		= 0;

    lastAO ++;
    return pAO;// ++;
}

void vUnRegistActiveObject ( ActiveObject_t * AO )
{
	for (int i= 0; i< lastAO; i++){
		if ( AO == &ActiveObjects[ i ] ){
			lastAO--;
			for ( int j = i; j< lastAO; j++){
				ActiveObjects[ i ].TH = ActiveObjects[ i+1 ].TH;
				ActiveObjects[ i ].manejadorEventos = ActiveObjects[ i+1 ].manejadorEventos;
				ActiveObjects[ i ].param_aux_1 = ActiveObjects[ i+1 ].param_aux_1;
				ActiveObjects[ i ].param_aux_2 = ActiveObjects[ i+1 ].param_aux_2;
			}
		}
	}
}

/*
void vInitActiveObjects ( void )
{
    Evento_t evn;

    for ( int ao = 0; ao < lastAO; ++ao )
    {
        evn.signal 		= SIG_INICIAR;
        evn.receptor 	= &ActiveObjects[ ao ]; 	//puntero al AO
        evn.pChar		= NULL;
        evn.largo		= 0;

        vDispatchEvent( &evn );
    }
}
*/

void vTaskEventDispatch ( void * noUsed )
{
// Manejo de eventos:
// Esta siempre bloqueada salvo que tenga un evento que manejar.
// En cuanto hay un evento, lo despacho a la FSM correspondiente SIN BLOQUEAR

    Evento_t evn;
    for( ;; )
    {
    	xQueueReceive( queueEvents, &evn, portMAX_DELAY );
    	vDispatchEvent( &evn );
    }
}

void vDispatchEvent( Evento_t *evento )
{
//  Llama al manejador de eventos correspondiente al módulo enviado
    fsm_ptr manejadorEventos = ( evento->receptor )->manejadorEventos;
    manejadorEventos( evento ); //Al receptor del evento le paso el evento
}

//envia el evento, toma los argumentos del evento en crudo
void vPutQueueEvent ( ActiveObject_t * receptor , Signal_t senhal, char * pChar, int largo )
{
    Evento_t evn;

    evn.receptor	= receptor;
    evn.signal		= senhal;
    evn.pChar       = pChar;
    evn.largo		= largo;

    xQueueSend( queueEvents, &evn, 0 );
    return;
}

/*==================[internal functions definition]==========================*/

static void vLowercaseConvert_mM( char *pMessage, int length )
{
	for( uint8_t ucIndex = 1; ucIndex < length; ucIndex++ )
	{
		if( pMessage[ ucIndex ] <= 'Z' )
			pMessage[ ucIndex ] += 32;
	}
}

static void vUppercaseConvert_mM( char *pMessage, int length  )
{
	for( uint8_t ucIndex = 1; ucIndex <= length; ucIndex++ )
	{
		if( pMessage[ ucIndex ] >= 'a' )
			pMessage[ ucIndex ] -= 32;
	}
}

/*==================[end of file]============================================*/
