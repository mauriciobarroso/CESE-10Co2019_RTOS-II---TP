/*
 * eventos.c
 *
 *  Created on: Apr 9, 2020
 *      Author: pablo
 */

#include "modulos.h"
#include "eventos.h"

xQueueHandle queueEvents;

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

void vPutQueueEvent ( Modulo_t * receptor , Signal_t senhal, char * pChar, int largo )
{
    Evento_t evn;

    evn.receptor	= receptor;
    evn.signal		= senhal;
    evn.pChar       = pChar;
    evn.largo		= largo;

    xQueueSend( queueEvents, &evn, 0 );
    return;
}


void vDispatchEvent( Evento_t *evento )
{
//  Llama al manejador de eventos correspondiente al módulo enviado

    fsm_ptr manejadorEventos = ( evento->receptor )->manejadorEventos;

    manejadorEventos( evento ); //Al receptor del evento le paso el evento
}




