/*
 * utiles.h
 *
 *  Created on: Mar 26, 2020
 *      Author: pablo
 */

#ifndef UTILES_H_
#define UTILES_H_

#include "sapi.h"

#define DEBUG

#ifdef DEBUG
	#define PRINT_DEBUG( x ) printf x
    #define LED_DEBUG( x )   gpioWrite x
#else
	#define PRINT_DEBUG(x) do {} while (0)
	#define LED_DEBUG(x)   do {} while (0)
#endif

typedef void(*taskfuncPtr_t) ( void * ); // puntero a task function

//Declaraciones de funciones

	void uartCallBackFunction( void * );
	void initDecodeCode      ( void );

	void srvUartRxReady      ( void * );

	static uint8_t loadMsgArr ( void );
	static bool_t  msgValidar ( uint8_t * );
	static void    msgComponer( uint8_t * );

	static void    rcvClear   ( void );

	static uint8_t strlen0    ( uint8_t * );

	static uint8_t uCaseLcase ( uint8_t, bool_t );

	static uint8_t crcCharToByte( uint8_t, uint8_t );
	static void    crcByteToChar( uint8_t, uint8_t * );

#endif /* UTILES_H_ */
