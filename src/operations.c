/* Copyright 2020, Mauricio Barroso
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* Date: 19/03/20 */

/*==================[inclusions]============================================*/

#include "operations.h"
#include "eventos.h"
#include "mM_module.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/



/*==================[internal functions declaration]=========================*/

static void vLowercaseConvert( MessageData_t *pxMessage );
static void vUppercaseConvert( MessageData_t *pxMessage );

/*==================[external functions definition]=========================*/

void vEventManager_Op ( Evento_t *evn )
{
    switch( evn->signal )
    {
		case SIG_INICIAR:
			//Ok!!
			break;
		case SIG_OK_CONVERSION_m:
			gpioToggle( LED1 );
			vTaskDelete( vTaskModulo_m_Handle );
			break;
		case SIG_OK_CONVERSION_M:
			gpioToggle( LED2 );
			vTaskDelete( vTaskModulo_M_Handle );
			break;
		default:
			//Ups!!
			break;
    }
}



void vOperationError( MessageData_t *pxMessage)
{
	strcpy( pxMessage->pucBlock, "ERROR" );
	pxMessage->ucLength = 5;
}

void vOperationSelect( MessageData_t *pxMessage )
{
	switch( pxMessage->pucBlock[ 0 ] )
	{
		case 'm':
			//vLowercaseConvert( pxMessage );
			xTaskCreate( vTaskModulo_m, "vTaskModulo_m", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 4, vTaskModulo_m_Handle );
	        vPutQueueEvent( Modulo_m, SIG_CONVERTIR_LOWERCASE, pxMessage->pucBlock, pxMessage->ucLength );
			break;
		case 'M':
			//vUppercaseConvert( pxMessage );
			xTaskCreate( vTaskModulo_M, "vTaskModulo_M", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 4, vTaskModulo_M_Handle );
			vPutQueueEvent( Modulo_M, SIG_CONVERTIR_UPPERCASE, pxMessage->pucBlock, pxMessage->ucLength );
			break;
		default:
			vOperationError( pxMessage );
			break;
	}
}

static void vLowercaseConvert( MessageData_t *pxMessage )
{
	for( uint8_t ucIndex = 1; ucIndex < pxMessage->ucLength; ucIndex++ )
	{
		if( pxMessage->pucBlock[ ucIndex ] <= 'Z' )
			pxMessage->pucBlock[ ucIndex ] += 32;
	}
}

static void vUppercaseConvert( MessageData_t *pxMessage )
{
	for( uint8_t ucIndex = 1; ucIndex <= pxMessage->ucLength; ucIndex++ )
	{
		if( pxMessage->pucBlock[ ucIndex ] >= 'a' )
			pxMessage->pucBlock[ ucIndex ] -= 32;
	}
}

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
