/*
 * operations.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

/*==================[inclusions]============================================*/

#include "operations.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/



/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]=========================*/

void vOperationLowercase( void *pvParameters )
{
	UartPacket_t *pxPacket = ( UartPacket_t * )pvParameters;

	for( uint8_t ucIndex = 1; ucIndex < pxPacket->ucLength; ucIndex++ )
	{
		if( pxPacket->pucBlock[ ucIndex ] <= 'Z' )
			pxPacket->pucBlock[ ucIndex ] += CONVERSION_FACTOR;
	}
}

void vOperationUppercase( void *pvParameters )
{
	UartPacket_t *pxPacket = ( UartPacket_t * )pvParameters;

	for( uint8_t ucIndex = 1; ucIndex <= pxPacket->ucLength; ucIndex++ )
	{
		if( pxPacket->pucBlock[ ucIndex ] >= 'a' )
			pxPacket->pucBlock[ ucIndex ] -= CONVERSION_FACTOR;
	}
}

void vOperationUpperLowercase( void *pvParameters )
{
	UartPacket_t *pxPacket = ( UartPacket_t * )pvParameters;

	for( uint8_t ucIndex = 1; ucIndex <= pxPacket->ucLength; ucIndex++ )
	{
		if( pxPacket->pucBlock[ ucIndex ] >= 'a' && ucIndex % 2 )
			pxPacket->pucBlock[ ucIndex ] -= CONVERSION_FACTOR;
		else if( pxPacket->pucBlock[ ucIndex ] <= 'Z' && !( ucIndex % 2 ))
			pxPacket->pucBlock[ ucIndex ] += CONVERSION_FACTOR;

	}
}

void vOperationError( void *pvParameters )
{
	UartPacket_t *pxPacket = ( UartPacket_t * )pvParameters;

	strcpy( pxPacket->pucBlock, "ERROR" );
	pxPacket->ucLength = 5;
}

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
