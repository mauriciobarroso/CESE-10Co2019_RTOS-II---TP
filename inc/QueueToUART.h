/*
 * queueToUART.h
 *
 *  Created on: Mar 25, 2020
 *      Author: pablo
 */

#include "sapi.h"

#ifndef QUEUETOUART_H_
#define QUEUETOUART_H_

//#define USE_QM  //usar el Quantum Leaps Memory tool Management

//Declaraciones de Funciones

	void initQueueToUART( void );

	void updQueueToUART ( void * );

	void sendQueueToUART( uint8_t * );

	static uint8_t 	strlen0		  ( uint8_t * );

	static void 	copiarStrToStr( uint8_t *,  uint8_t * );



#endif /* QUEUETOUART_H_ */
