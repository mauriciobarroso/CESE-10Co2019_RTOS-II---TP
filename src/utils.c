/*
 * utils.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

/*==================[inlcusions]============================================*/

#include "utils.h"
#include "string.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

static uint8_t crc8_small_table[16] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d
};

/*==================[external data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static eMessageError_t eMessageErrorType( UartPacket_t *pxPacket );

/*==================[external functions definition]=========================*/

void vExtractMessage( UartPacket_t *pxPacket )
{
	for( uint8_t i = 0; i <= pxPacket->ucLength - 2; i++ )	// 4 hardcodeado
		pxPacket->pucBlock[ i ] = pxPacket->pucBlock[ i + 1 ];
	/* se reduce el largo del string en 3 */
	pxPacket->ucLength -= 2;
}



uint8_t crc8_init(void)
{
    return 0xff;
}

uint8_t crc8_calc(uint8_t val, void *buf, int cnt)
{
	int i;
	uint8_t *p = buf;

	for (i = 0; i < cnt; i++) {
		val ^= p[i];
		val = (val << 4) ^ crc8_small_table[val >> 4];
		val = (val << 4) ^ crc8_small_table[val >> 4];
	}
	return val;
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

bool_t bCheckCrc( UartPacket_t *pxPacket )
{
	uint8_t ucCrc1;
	uint8_t ucCrc2;

	ucCrc1 = crc8_calc( 0, pxPacket->pucBlock, pxPacket->ucLength - 2 );

	ucCrc2 = ucCrcCharToByte( pxPacket->pucBlock[ pxPacket->ucLength - 2 ], pxPacket->pucBlock[ pxPacket->ucLength - 1 ] );

	if( ucCrc1 != ucCrc2 )
		return FALSE;

	pxPacket->ucLength -= 2;

	return TRUE;
}

void vAddStartAndEndCharacters( UartPacket_t *pxPacket )
{
	for( uint8_t i = pxPacket->ucLength; i != 0; i-- )
		pxPacket->pucBlock[ i ] = pxPacket->pucBlock[ i - 1 ];

	pxPacket->ucLength += 2;

	pxPacket->pucBlock[ 0 ] = '(';
	pxPacket->pucBlock[ pxPacket->ucLength - 1 ] = ')';
}

void vAddCrc( UartPacket_t *pxPacket )
{
	uint8_t crc = crc8_calc( 0, pxPacket->pucBlock, pxPacket->ucLength );
	uint8_t bCrc[2];
	vCrcByteToChar( crc, bCrc );

	pxPacket->ucLength += 2;
	pxPacket->pucBlock[ pxPacket->ucLength - 2 ] = bCrc[1];
	pxPacket->pucBlock[ pxPacket->ucLength - 1 ] = bCrc[0];
}

bool_t bCheckPacket( UartPacket_t *pxPacket )
{
	switch( eMessageErrorType( pxPacket ) )
	{
		case ERROR_1:
			strcpy( pxPacket->pucBlock, " ERROR_1" );
			pxPacket->ucLength = 7;
			return FALSE;

		case ERROR_2:
			strcpy( pxPacket->pucBlock, " ERROR_2" );
			pxPacket->ucLength = 7;
			return FALSE;

		default:
			break;
	}

	return TRUE;
}

bool_t bCheckCharacters( UartPacket_t *pxPacket )
{
    for( uint8_t ucIndex = 0; ucIndex < pxPacket->ucLength; ucIndex++ )
    {
    	if( !( ( pxPacket->pucBlock[ ucIndex ] >= 'A' && pxPacket->pucBlock[ ucIndex ] <= 'Z' ) || ( pxPacket->pucBlock[ ucIndex ] >= 'a' && pxPacket->pucBlock[ ucIndex ] <= 'z' ) ) )
    		return FALSE;
    }

    return TRUE;
}

/*==================[internal functions definition]==========================*/


static eMessageError_t eMessageErrorType( UartPacket_t *pxPacket )
{
    if ( pxPacket->pucBlock[ 0 ] != '(' )
    	return ERROR_1;

    if ( pxPacket->pucBlock[ pxPacket->ucLength - 1 ] != ')' )
    	return ERROR_2;

    return NO_ERROR;
}


/*==================[end of file]============================================*/
