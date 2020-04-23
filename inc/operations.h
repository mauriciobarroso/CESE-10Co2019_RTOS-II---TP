/*
 * operations.h
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

/*==================[inclusions]=============================================*/

#include <stdint.h>

#include "activeObject.h"
#include "string.h"
#include "uartDriver.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void vEventManager_Op ( Evento_t * );

void vOperationError( MessageData_t *pxMessage );
void vOperationSelect( MessageData_t *pxMessage );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _OPERATIONS_H_ */
