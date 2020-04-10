/*
 * eventos.h
 *
 *  Created on: Apr 9, 2020
 *      Author: pablo
 */

#ifndef EVENTOS_H_
#define EVENTOS_H_

#include "modulos.h"

void vTaskEventDispatch ( void * );
void vPutQueueEvent( Modulo_t *, Signal_t, char *, int );
void vDispatchEvent( Evento_t * );


#endif /* EVENTO_H_ */
