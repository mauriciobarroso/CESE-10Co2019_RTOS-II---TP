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

#include "check.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static bool_t bCheckCrc( char *pucString );
static eMessageError_t eMessageErrorType( char *pucString );

/*==================[external functions definition]=========================*/

bool_t bCheckPacket( char *pucString )
{

	if( !bCheckCrc( pucString ) )
		return FALSE;

	switch( eMessageErrorType( pucString) )
	{
		case ERROR_1:
			strcpy( pucString, " ERROR_1" );
			pucString[ 0 ] = 7;
			return FALSE;

		case ERROR_2:
			strcpy( pucString, " ERROR_2" );
			pucString[ 0 ] = 7;
			return FALSE;

		/*case ERROR_3:
			strcpy( pucString, " ERROR_3" );
			pucString[ 0 ] = 7;
			return FALSE;*/

		default:
			break;
	}

	return TRUE;
}

/*==================[internal functions definition]==========================*/

static bool_t bCheckCrc( char *pucString )
{
	uint8_t ucStringLenght;
	uint8_t ucCrc1;
	uint8_t ucCrc2;

	ucStringLenght = pucString[ 0 ];

	ucCrc1 = crc8_calc( 0, pucString, ucStringLenght - 2 );

	ucCrc2 = ucCrcCharToByte( pucString[ ucStringLenght - 2 ], pucString[ ucStringLenght - 1 ] );

	if( ucCrc1 != ucCrc2 )
		return FALSE;

	return TRUE;
}

static eMessageError_t eMessageErrorType( char *pucString )
{
    uint8_t ucStringLength = pucString[ 0 ];

    if ( pucString[ 1 ] != '(' )
    	return ERROR_1;

    if ( pucString[ ucStringLength ] != ')' )
    	return ERROR_2;

    for( uint8_t ucIndex = 2; ucIndex <= ucStringLength - 3; ucIndex++ )
    {
    	if( !( ( pucString[ ucIndex ] >= 'A' && pucString[ ucIndex ] <= 'Z' ) || ( pucString[ ucIndex ] >= 'a' && pucString[ ucIndex ] <= 'z' ) ) )
    		return ERROR_3;
    }

    return NO_ERROR;
}

/*==================[end of file]============================================*/
