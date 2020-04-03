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

/*==================[inlcusions]============================================*/

#include "utils.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/

void vExtractMessage( char *pucString )
{
	uint8_t ucStringLength;

	ucStringLength = pucString[ 0 ];

	for( uint8_t i = 1; i <= ucStringLength - 4; i++ )
		pucString[ i ] = pucString[ i + 1 ];
	/* se reduce el largo del string en 3 */
	pucString[ 0 ] -= 4;
}

void vCrcByteToChar( uint8_t ucCrc, uint8_t *pucCrc )
{
	pucCrc[ 0 ] = ucCrc & 0b00001111;	//elimino el 2do nibble

	if ( pucCrc[ 0 ] < 10 )
		pucCrc[ 0 ] += 48;
	else
		pucCrc[ 0 ] += 55;   //+87 para lcase

	pucCrc[ 1 ] = ucCrc >> 4;

	if ( pucCrc[ 1 ] < 10 )
		pucCrc[ 1 ] += 48;
	else
		pucCrc[ 1 ] += 55;   //+87 para lcase
}

uint8_t ucCrcCharToByte( uint8_t ucCrc1, uint8_t ucCrc0)
{
	uint8_t ucCrc;

	if( ( ucCrc1 >= 'A' ) && ( ucCrc1 <= 'F' ) )	ucCrc = ( ucCrc1 - 55 ) * 16;
	if( ( ucCrc1 >= 'a' ) && ( ucCrc1 <= 'f' ) )	ucCrc = ( ucCrc1 - 87 ) * 16;
	if( ( ucCrc1 >= '0' ) && ( ucCrc1 <= '9' ) )	ucCrc = ( ucCrc1 - 48 ) * 16;

	if( ( ucCrc0 >= 'A' ) && ( ucCrc0 <= 'F' ) )	ucCrc = ( ucCrc0 - 55 ) + ucCrc;
	if( ( ucCrc0 >= 'a' ) && ( ucCrc0 <= 'f' ) )	ucCrc = ( ucCrc0 - 87 ) + ucCrc;
	if( ( ucCrc0 >= '0' ) && ( ucCrc0 <= '9' ) )	ucCrc = ( ucCrc0 - 48 ) + ucCrc;

	return ucCrc;
}


void vAddStartAndEndCharacters( char *pucString )
{
	for( uint8_t i = pucString[ 0 ]; i > 0; i-- )
		pucString[ i + 1 ] = pucString[ i ];

	pucString[ 0 ] += 2;
	pucString[ 1 ] = '(';
	pucString[ pucString[ 0 ] ] = ')';
}

void vAddCrc( char *pucString )
{
	uint8_t crc = crc8_calc( 0, pucString, pucString[ 0 ] );
	uint8_t bCrc[2];
	vCrcByteToChar( crc, bCrc );

	pucString[ 0 ] += 2;
	pucString[ pucString[ 0 ] - 2 ] = bCrc[1];
	pucString[ pucString[ 0 ] - 1 ] = bCrc[0];
}

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]=========================*/

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
