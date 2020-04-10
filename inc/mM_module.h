/*
 * mM_module.h
 *
 *  Created on: Apr 9, 2020
 *      Author: pablo
 */

#ifndef _MM_MODULE_H_
#define _MM_MODULE_H_

#include "modulos.h"
#include "operations.h"

void vTaskModulo_m ( void * );
void vTaskModulo_M ( void * );

static void vSendEvent_m( char *, int );
static void vSendEvent_M( char *, int );

void vEventManager_m ( Evento_t * );
void vEventManager_M ( Evento_t * );

#endif /* _MM_MODULE_H_ */
