/*
 * queueToUART.c
 *
 *  Created on: Mar 25, 2020
 *      Author: Pablo J.C. Alonso Castillo
 */

/*==================[inclusions]=============================================*/

#include "QueueToUART.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// sAPI
#include "sapi.h"

#include "qmpool2.h"

QueueHandle_t myQueueToUARTHandle = NULL;

/*==================[Declaraciones y variables asociadas al Quantum Leaps Mem Pool]=========*/

#ifdef USE_QM
QMPool miMemPool;
uint8_t miMemPoolSto[150*1]; // 50 bloques de 1 byte
#endif

// Inicialización del Queue y la memoria
void initQueueToUART( void )
{
#ifdef USE_QM
	 // Inicializar Pool de memoria Quantum Leaps
	 QMPool_init(&miMemPool,
	             miMemPoolSto,
	             sizeof(miMemPoolSto),
	             1U);  /* bloques 1 bytes cada uno */
#endif

	 myQueueToUARTHandle = xQueueCreate( 6,  sizeof( void * ) );
}

// Update del Queue y envio al puerto serie
void updQueueToUART( void * taskParmPtr )
{
	void * ptr;

	while (1){
		if ( xQueueReceive( myQueueToUARTHandle, &ptr, 1000 / portTICK_RATE_MS ) ){

			printf( "--> %s\n", ptr );

			#ifdef USE_QM
				QMPool_put( &miMemPool, ptr );
			#else
				vPortFree( ptr );
			#endif
		}
		else {
			//puts( "falla al recibir dato del queue" );
		}
	}
}


void sendQueueToUART( uint8_t * strPtr)
{
	void * ptr;

	#ifdef USE_QM
		if ( strlen0( strPtr ) > QMPool_getMin( &miMemPool ) ){
			//error mensaje demasiado largo
			return;
		}
	#endif


	#ifdef USE_QM
		ptr = QMPool_get( &miMemPool, strlen0( strPtr ) );  //Total de strlen0( strPtr ) bloques de 1 Byte
	#else
		ptr = pvPortMalloc( strlen0( strPtr ) );
	#endif

	if (ptr){

		copiarStrToStr( strPtr, (uint8_t *) ptr );

		xQueueSend( myQueueToUARTHandle, &ptr,  (TickType_t) 100);

   }else{
	   //Falla alocación de memoria

   }

}


/* strlen0: retorna la longitud del string s incluido el 0 final */
static uint8_t strlen0( uint8_t * s )
{
    uint8_t n;

    for (n = 0; *s != '\0'; s++)
        n++;
    return n + 1;
}

/*Copia el strIn en el strOut, solo considera la longitud del inicial y le da sin asco */
static void copiarStrToStr( uint8_t * strIn,  uint8_t * strOut)
{
	uint8_t strLng = strlen0( strIn );  // recordar que incluye el 0 final

	for( uint8_t i = 0; i < strLng ; i++){
		*( strOut + i ) =  *( strIn + i );
	}
}

