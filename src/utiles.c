/*
 * utiles.c
 *
 *  Created on: Mar 26, 2020
 *      Author: pablo
 */
#include "QueueToUART.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "sapi.h"
#include "utiles.h"

#include "crc8.h"


#define MAX_SIZE_RCV_MSG  100  //tamaño máximo del mensaje aceptable

/*==================[definiciones de datos internos]=========================*/

static void * ptrRcv; //puntero a elementos recibidos
static void * ptrMsg; //puntero a elementos del mensaje

static uint8_t msgIndex = 0;
static uint8_t rcvIndex = 0;

void uartCallBackFunction( void * ptr){
	//TO-DO
}

void srvUartRxReady( void * ptr)
{
	uint8_t rcvData;

	while(1){

		if ( uartRxReady( UART_USB ) ){

			gpioWrite( LED1, !gpioRead( LED1 ) ); //Señal de vida
			gpioWrite( LED2,  gpioRead( LED1 ) );

			rcvData = uartRxRead( UART_USB );

			//cuando se recibe un '(' se lo coloca como primer elemento del arreglo
			//un '(' resetea a 0 el arreglo de recepción
			if ( rcvData == '(' ) rcvIndex = 0;    //reseteo el rcvIndex cuando recibo un '('

			* ( ( char * ) ptrRcv + rcvIndex ) = rcvData; //guardo los elementos recibidos

			rcvIndex++;

			if ( rcvIndex >= MAX_SIZE_RCV_MSG ) rcvIndex = 0;  //vuelvo al inicio, si esto sucede el mensaje era demasiado largo

			if ( rcvData == ')' ){

				msgIndex = loadMsgArr(); //reviso para ver si hay algo entre (-)

				rcvClear(); //limpio el buffer

				if ( msgIndex < 7  ){  //los mensajes válidos tienen por lo menos 7 de extensión

					PRINT_DEBUG(( "\n" ));

					#ifdef DEBUG
					      sendQueueToUART ( ptrMsg );
					#endif
				}

				if ( msgIndex > 6 ){

					PRINT_DEBUG(( "\n" ));

					#ifdef DEBUG
						sendQueueToUART ( ptrMsg );
					#endif

				   if ( msgValidar( ptrMsg ) ){
					   PRINT_DEBUG(( " OK! \n" ));

					   msgComponer( ptrMsg );

					   sendQueueToUART ( ptrMsg );

				   }else{

					   PRINT_DEBUG(( " MAL! \n" ));
				   }
				}
			}
		}
	}
}

//A esta función se la llama cuando se recibe un ')'
//Se copia en el arreglo apuntado por prtMsg desde el arreglo apuntado por ptrRcv
//comenzando en el último '(' recibido y terminando en el primer ')' recibido
//Recordar que siempre el primer elemento debe ser '(' si es un arreglo del tipo '(._._._etc._._)'
//devuelve el N° de elementos cargados al arreglo msgArr si este es cerrado por () o 0 si no lo es
//El mínimo para un arreglo cerrado es 3: [ (, ), \0 ]
//para que el msg sea examinado debe haber por lo menos 7: [ (, m/M, Data, crc1, crc0, ), \0 ]
static uint8_t loadMsgArr( void ){

	uint8_t j = 0;

	bool_t bMsgInicio = FALSE;

	if ( *((char*)ptrRcv) != '(' ) return 0;  //siempre el primer elemento debe ser '('

	ptrMsg = pvPortMalloc( rcvIndex + 1 );    //se le debe sumar el terminador '\0'


	for ( uint8_t i = 0; i < rcvIndex; i++ )
	{
		*( ( uint8_t * ) ptrMsg + i ) = *( ( uint8_t * ) ptrRcv + i ) ;
	}

	//al final ')' se le suma un terminador \0
	*( ( uint8_t * ) ptrMsg + rcvIndex ) = '\0';

	return rcvIndex + 1;			//devuelvo el N° de elementos del arreglo con el 0 final
}


static bool_t msgValidar( uint8_t * rcvArr ){

	uint8_t largo = 0;
	largo = strlen0( rcvArr );

	//Verifico el largo, los mensajes válidos tienen por lo menos 7 de extensión
	//[ (, m/M, Data, crc1, crc0, ), \0 ]
	if ( largo < 7 ) return FALSE;

	//Verifico OPT
	if ( ( rcvArr[ 1 ] != 'm' ) && ( rcvArr[ 1 ] != 'M' ) ) return FALSE;

	//Verifico solo alfabéticos
	for (uint8_t i = 2; i < largo - 4; i++ ){
		if ( !( ( ( rcvArr[ i ] >= 'A' ) && ( rcvArr[ i ] <= 'Z' ) ) ||
			    ( ( rcvArr[ i ] >= 'a' ) && ( rcvArr[ i ] <= 'z' ) ) ) ){
			return FALSE;
		}
	}

	//Verifico crc8
	//uint8_t * ptrCrc = rcvArr + 2;

	uint8_t valCrc = crc8_calc( 0 , rcvArr + 2, largo - 4 - 2 );// 4 del crc + ) + \0, 2 del ( + opt

	PRINT_DEBUG(( "crc8 = %X ", valCrc ));

	if ( crcCharToByte( rcvArr[ largo - 4 ], rcvArr[ largo - 3 ]) != valCrc ) return FALSE;

	//Si no retornó en los anteriores quiere decir que está TRUE
	return TRUE;
}


void initDecodeCode( void )
{
	ptrRcv = pvPortMalloc( MAX_SIZE_RCV_MSG );

	rcvClear();

	msgIndex = 0;
}

//limpia el arreglo de recepción, en realidad no hace falta, solo es
//para que al debuggear solo se vea el último msg recibido sin escoria
static void rcvClear( void )
{
	rcvIndex = 0;

	for ( uint8_t i = 0; i < MAX_SIZE_RCV_MSG; i++ )
	{
		  * ( ( char * ) ptrRcv + i ) = 0;
	}
}


/* strlen1: retorna la longitud del string s incluido el 0 final */
static uint8_t strlen0( uint8_t * s )
{
    uint8_t n;

    for (n = 0; *s != '\0'; s++)
        n++;
    return n + 1;
}

//Toma dos bytes ascii que representan un Hex y lo devuelven como un int
static uint8_t crcCharToByte( uint8_t crc1, uint8_t crc0)
{
	uint8_t crc;

	if( ( crc1 >= 'A' ) && ( crc1 <= 'F' ) )		crc = ( crc1 - 55 ) * 16;
	if( ( crc1 >= 'a' ) && ( crc1 <= 'f' ) )		crc = ( crc1 - 87 ) * 16;
	if( ( crc1 >= '0' ) && ( crc1 <= '9' ) )		crc = ( crc1 - 48 ) * 16;

	if( ( crc0 >= 'A' ) && ( crc0 <= 'F' ) )		crc = ( crc0 - 55 ) + crc;
	if( ( crc0 >= 'a' ) && ( crc0 <= 'f' ) )		crc = ( crc0 - 87 ) + crc;
	if( ( crc0 >= '0' ) && ( crc0 <= '9' ) )		crc = ( crc0 - 48 ) + crc;

	return crc;
}
//Toma un byte que representan un Hex y lo devuelven como dos bytes ascii
static void crcByteToChar( uint8_t crc, uint8_t * x )
{
	x [ 0 ] = crc & 0b00001111;	//elimino el 2do nibble

	if ( x[ 0 ] < 10 ){
		 x[ 0 ] += 48;
	}else{
		 x[ 0 ] += 55;   //+87 para lcase
	}

	x [ 1 ] = crc >> 4;

	if ( x[ 1 ] < 10 ){
		 x[ 1 ] += 48;
	}else{
	     x[ 1 ] += 55;   //+87 para lcase
	}
}


//procesa el mensaje para devolverlo.
//convierte el mensaje en LCases o UCases según el opt
//calcula el crc8 y vuelve a conformarlo como dos bytes ascii
static void msgComponer( uint8_t * rcvArr )
{
	uint8_t largo = 0;
	largo = strlen0( rcvArr );

	uint8_t opt = rcvArr[ 1 ];   //rescato el opt

	for ( uint8_t i = 2; i < largo - 4; i++  )  //convierto a LCase o UCase según corresponda
	{
		switch (opt){
			case 'M':
				rcvArr[ i ] = uCaseLcase( rcvArr[ i ], TRUE);
			break;
			case 'm':
				rcvArr[ i ] = uCaseLcase( rcvArr[ i ], FALSE);
			break;
		}
	}

    //calculo el crc8
	uint8_t valCrc = crc8_calc( 0 , rcvArr + 2, largo - 4 - 2 );// 4 del crc + ) + \0, 2 del ( + opt

	//covierto el crc8 a bytes ascii
	uint8_t losChar[2];
	crcByteToChar( valCrc, losChar );

	//cololo el crc8 ascii en el arreglo
	rcvArr[ largo - 3 ] = losChar[0];
	rcvArr[ largo - 4 ] = losChar[1];
}

//pasa elchar a UCase si bElCase =  TRUE
//pasa elchar a LCase si bElCase =  FALSE
static uint8_t uCaseLcase( uint8_t elChar, bool_t bElCase)
{
	if ( bElCase ) {       //convierto a UCase
		if ( ( elChar >= 'A' ) && ( elChar <= 'Z' ) ){
			return elChar; //no hago nada
		}else{
			return elChar - 32 ; //resto 32
		}
	}else{				   //convierto a LCase
	    if ( ( elChar >= 'a' ) && ( elChar <= 'z' ) ){
	    	return elChar; //no hago nada
	    }else{
	    	return elChar + 32 ; //sumo 32
	    }
	}
}















