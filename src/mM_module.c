/*
 * mM_module.c
 *
 *  Created on: Apr 9, 2020
 *      Author: pablo
 *
 */

#include "sapi.h"
#include "eventos.h"
#include "modulos.h"
#include "mM_module.h"

static void vLowercaseConvert_mM( char * , int );
static void vUppercaseConvert_mM( char * , int );

void vTaskModulo_m ( void * noUsed )
{
// Manejo de eventos:
// Esta siempre bloqueada salvo que tenga un evento que manejar.
// En cuanto hay un evento, lo despacho a la FSM correspondiente SIN BLOQUEAR

    Evento_t evn;
    for( ;; )
    {
    	xQueueReceive( queueEvents, &evn, portMAX_DELAY );
    	//vLowercaseConvert_mM( evn.pChar, evn.largo );
    	//EnviarEvento_m( evn.pChar, evn.largo );
    	vDispatchEvent( &evn );
    }
}

void vTaskModulo_M ( void * noUsed )
{
// Manejo de eventos:
// Esta siempre bloqueada salvo que tenga un evento que manejar.
// En cuanto hay un evento, lo despacho a la FSM correspondiente SIN BLOQUEAR

    Evento_t evn;
    for( ;; )
    {
    	xQueueReceive( queueEvents, &evn, portMAX_DELAY );
      	//vUppercaseConvert_mM( evn.pChar, evn.largo );
        //EnviarEvento_M( evn.pChar, evn.largo );
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
			vSendEvent_m( evn->pChar, evn->largo );
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
		vSendEvent_M( evn->pChar, evn->largo );
		break;
	default:
		//Ups!!
		break;
    }
}


static void vSendEvent_m ( char *pChar, int largo )
{
    vPutQueueEvent( Modulo_Op, SIG_OK_CONVERSION_m, pChar, largo );
}

static void vSendEvent_M ( char *pChar, int largo )
{
    vPutQueueEvent( Modulo_Op, SIG_OK_CONVERSION_M, pChar, largo );
}

static void vLowercaseConvert_mM( char *pMessage, int length )
{
	for( uint8_t ucIndex = 1; ucIndex < length; ucIndex++ )
	{
		if( pMessage[ ucIndex ] <= 'Z' )
			pMessage[ ucIndex ] += CONVERSION_FACTOR;
	}
}

static void vUppercaseConvert_mM( char *pMessage, int length  )
{
	for( uint8_t ucIndex = 1; ucIndex <= length; ucIndex++ )
	{
		if( pMessage[ ucIndex ] >= 'a' )
			pMessage[ ucIndex ] -= CONVERSION_FACTOR;
	}
}




