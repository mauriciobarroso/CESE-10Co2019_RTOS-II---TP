/*
 * utils.h
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

#ifndef _UTILS_H_
#define _UTILS_H_

/*==================[inclusions]=============================================*/

#include <stdint.h>
#include "uartDriver.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

typedef enum
{
	ERROR_1,
	ERROR_2,
	ERROR_3,
	NO_ERROR,
} eMessageError_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void vExtractMessage( UartPacket_t *pxMessage );
uint8_t crc8_init( void );
uint8_t crc8_calc( uint8_t val, void *buf, int cnt );
void vCrcByteToChar( uint8_t ucCrc, uint8_t *puc );
uint8_t ucCrcCharToByte( uint8_t ucCrc1, uint8_t ucCrc0);
bool_t bCheckCrc( UartPacket_t *pxMessage );
void vAddStartAndEndCharacters( UartPacket_t *pxMessage );
void vAddCrc( UartPacket_t *pxMessage );
bool_t bCheckPacket( UartPacket_t *pxMessage );
bool_t bCheckCharacters( UartPacket_t *pxPacket );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _UTILS_H_ */
