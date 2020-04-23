/*
 * uartIRQ.h
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

#ifndef _UART_IRQ_H_
#define _UART_IRQ_H_

/*==================[inclusions]=============================================*/

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "task.h"

#include "sapi.h"

#include "uartDriver.h"
#include "utils.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void vRxTimeOutHandler( TimerHandle_t xTimer );
void vTxTimeOutHandler( TimerHandle_t xTimer );
bool_t bRxInterruptEnable( UartInstance_t *pxUartInstance );
bool_t bTxInterruptEnable( UartInstance_t *pxUartInstance );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _UART_IRQ_H_ */
