/*
 * modulos.h
 *
 *  Created on: Apr 10, 2020
 *      Author: pablo
 */

#ifndef MODULOS_H_
#define MODULOS_H_

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "modulos.h"

typedef struct Modulo_t Modulo_t;
typedef struct Evento_t Evento_t;

typedef void ( *fsm_ptr ) ( Evento_t * );

typedef struct Modulo_t
{
    fsm_ptr	manejadorEventos;
    int 	param_aux_1;
    int 	param_aux_2;
} Modulo_t;


typedef struct Evento_t
{
    Modulo_t *	receptor;
    int			signal;
    char *		pChar;
    int			largo;
} Evento_t;


typedef enum
{
    SIG_INICIAR   			=0,
	SIG_CONVERTIR_UPPERCASE	  ,
	SIG_CONVERTIR_LOWERCASE   ,
	SIG_FINALIZAR			  ,
	SIG_OK_CONVERSION         ,
	SIG_OK_CONVERSION_m       ,
	SIG_OK_CONVERSION_M       ,
} Signal_t;

Modulo_t * 	xRegistModule    ( fsm_ptr manejadorEventos );
void        vInitModules     ( void );

extern xQueueHandle queueEvents;

//Aqui debo declarar como extern todos los módulos que use
extern Modulo_t * Modulo_m;
extern Modulo_t * Modulo_M;
extern Modulo_t * Modulo_Op;

extern TaskHandle_t vTaskModulo_m_Handle;
extern TaskHandle_t vTaskModulo_M_Handle;

#endif /* MODULOS_H_ */



