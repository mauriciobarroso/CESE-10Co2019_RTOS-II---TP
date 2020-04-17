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
#include "activeObject.h"

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
			vDeleteActiveObject( ActiveObject_m );
			gpioToggle( LED2 );
			break;
		case SIG_OK_CONVERSION_M:
			vDeleteActiveObject( ActiveObject_M );
			gpioToggle( LED2 );
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
			if ( ! bCreateActiveObject_m( pxMessage ) ){
				vOperationError( pxMessage );
			}
			break;
		case 'M':
			if ( ! bCreateActiveObject_M( pxMessage ) ){
				vOperationError( pxMessage );
			}
			break;
		default:
			vOperationError( pxMessage );
			break;
	}
}


/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
