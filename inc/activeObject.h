/*
 * activeObject.h
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

#ifndef _ACTIVEOBJECT_H_
#define _ACTIVEOBJECT_H_

/*==================[inclusions]=============================================*/

#include <stdint.h>
#include "string.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "uartDriver.h"
#include "operations.h"
#include "events.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define MAX_ACTIVE_OBJECTS_NUMBER	10	// cantidad máxima de AOs
#define LENGTH_QUEUE_AO				10	// longitud de la cola del AO

/*==================[typedef]================================================*/

typedef struct
{
	bool_t bAlive;				// indicador de existencia del AO
	QueueHandle_t xQueue;		// cola de recepción del AO
	TaskFunction_t xCallback;	// función de callback del AO
	UBaseType_t uxPriority;		// prioridad del thread del AO
	TaskHandle_t xTask;			// handle del thread del AO
} ActiveObjectConf_t;

typedef struct
{
	eEventType_t eEventType;				// tipo de evento
	ActiveObjectConf_t xActiveObjectConf;	// configuración del AO
} ActiveObject_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief Función para registrar AOs en el array de AOs
 *
 * @param pxActiveObjectConf[in] puntero a la configuraciín del AO
 * @param EventType[in] Tipo de evento para el que se crea el AO
 *
 * @return
 * 		- TRUE Éxito
 * 		- FALSE Número de AOs registrados fuera de rango
 */
bool_t bActiveObjectRegister( ActiveObjectConf_t *pxActiveObjectConf, eEventType_t EventType );

/**
 * @brief Función para crear AOs (thread y queue)
 *
 * @param pxActiveObjectConf[in] puntero a la configuraciín del AO
 *
 * @return
 * 		- TRUE Éxito
 * 		- FALSE Fallo al crear el AO (thread o queue)
 */
bool_t bActiveObjectCreate( ActiveObjectConf_t *pxActiveObjectConf );

/**
 * @brief Función para eliminar AOs (thread y queue)
 *
 * @param pxActiveObjectConf[in] puntero a la configuraciín del AO
 *
 */
void vActiveObjectDelete( ActiveObjectConf_t *pxActiveObjectConf );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _ACTIVEOBJECT_H_ */
