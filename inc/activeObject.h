/*
 * mM_module.h
 *
 *  Created on: Apr 9, 2020
 *      Author: pablo
 */

#ifndef _MM_AO_H_
#define _MM_AO_H_

#include "uartDriver.h"

typedef struct ActiveObject_t ActiveObject_t;
typedef struct Evento_t Evento_t;

typedef void ( *fsm_ptr ) ( Evento_t * );

typedef struct ActiveObject_t
{
    fsm_ptr	 	 manejadorEventos;
    TaskHandle_t TH;
    //QueueHandle_t QH
    int 	     param_aux_1;
    int 		 param_aux_2;
} ActiveObject_t;


typedef struct Evento_t
{
	ActiveObject_t * receptor;
    int				 signal;
    char *			 pChar;
    int			     largo;
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

//Aqui debo declarar como extern todos los Active Objects que use
extern ActiveObject_t * ActiveObject_m;
extern ActiveObject_t * ActiveObject_M;
extern ActiveObject_t * Modulo_Op;

//El Queue de los eventos:
extern xQueueHandle queueEvents;

//Funciones Específicas de cada AO

//ActiveObject_m
bool_t bCreateActiveObject_m( MessageData_t * );
void vTaskActiveObject_m ( void * );
void vEventManager_m ( Evento_t * );

//ActiveObject_m
bool_t bCreateActiveObject_M( MessageData_t * );
void vTaskActiveObject_M ( void * );
void vEventManager_M ( Evento_t * );

//Funciones Generales para todos los AO

ActiveObject_t * xRegistActiveObject( fsm_ptr manejadorEventos );
void			 vUnRegistActiveObject( ActiveObject_t * );
//void             vInitActiveObjects ( void );
void vTaskEventDispatch ( void * );
void vDispatchEvent     ( Evento_t * );

void vPutQueueEvent     ( ActiveObject_t *, Signal_t, char *, int );

void vDeleteActiveObject( ActiveObject_t *);


#endif /* _MM_AO_H_ */
