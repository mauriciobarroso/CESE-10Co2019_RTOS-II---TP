/*
 * activeObject.h
 *
 *  Created on: Apr 9, 2020
 *      Author: pablo
 */
#ifndef _EVENTS_H_
#define _EVENTS_H_

/*==================[inclusions]=============================================*/

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

typedef enum
{
	UNKNOW,
	UART_PACKET_LOWERCASE,
	UART_PACKET_UPPERCASE,
	UART_PACKET_UPPERLOWERCASE,
	UART_PACKET_INVERT,
	UART_PACKET_ERROR
} eEventType_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _EVENTS_H_ */
