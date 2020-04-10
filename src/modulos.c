/*
 * modulos.c
 *
 *  Created on: 08/04/2020
 *      Author: pablo
 */

#include "sapi.h"
#include "eventos.h"
#include "modulos.h"

#define 	MAX_MODULOS 20
Modulo_t 	modulos[MAX_MODULOS];
int 		lastModule = 0;


Modulo_t * xRegistModule ( fsm_ptr manejadorEventos )
{
    if( lastModule >= MAX_MODULOS )
    {
        return NULL;
    }

    Modulo_t * pModulo 			= &modulos[ lastModule ];
    pModulo->manejadorEventos	= manejadorEventos;
    pModulo->param_aux_1        = 0;
    pModulo->param_aux_2 		= 0;

    lastModule ++;
    return pModulo ++;
}

void vInitModules ( void )
{
    Evento_t evn;

    for ( int module = 0; module < lastModule; ++module )
    {
        evn.signal 		= SIG_INICIAR;
        evn.receptor 	= &modulos[ module ]; 	//puntero al módulo
        evn.pChar		= NULL;
        evn.largo		= 0;

        vDispatchEvent( &evn );
    }
}

