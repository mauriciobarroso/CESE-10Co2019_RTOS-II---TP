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
#include "string.h"

#include "uartDriver.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define CONVERSION_FACTOR	32

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void vOperationLowercase( void *pvParameters );
void vOperationUppercase( void *pvParameters );
void vOperationUpperLowercase( void *pvParameters );
void vOperationInvert( void *pvParameters );
void vOperationError( void *pvParameters );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _OPERATIONS_H_ */
