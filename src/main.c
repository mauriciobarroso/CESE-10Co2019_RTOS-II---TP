/*
 * main.c
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

/*==================[inclusions]============================================*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"

#include "activeObject.h"
#include "userTasks.h"

#include "operations.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/

/* instancia de la UART a usar */
UartInstance_t xUartInstance;

/* objetos activos a registrar */
ActiveObject_t xActiveObjectLowercase;
ActiveObject_t xActiveObjectUppercase;
ActiveObject_t xActiveObjectUpperLowercase;
ActiveObject_t xActiveObjectInvert;

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]=========================*/

int main( void )
{
   /* se inicializa la EDU-CIAA */
   boardConfig();

   /* se registra un objeto activo asociado con un evento y un callback para minusculizar */
   xActiveObjectLowercase.xActiveObjectConf.xCallback = vOperationLowercase;
   xActiveObjectLowercase.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   xActiveObjectLowercase.eEventType = UART_PACKET_LOWERCASE;
   bActiveObjectRegister( &xActiveObjectLowercase.xActiveObjectConf, xActiveObjectLowercase.eEventType );

   /* se registra un objeto activo asociado con un evento y un callback para mayusculizar */
   xActiveObjectUppercase.xActiveObjectConf.xCallback = vOperationUppercase;
   xActiveObjectUppercase.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   xActiveObjectUppercase.eEventType = UART_PACKET_UPPERCASE;
   bActiveObjectRegister( &xActiveObjectUppercase.xActiveObjectConf, xActiveObjectUppercase.eEventType );

   /* se registra un objeto activo asociado con un evento y un callback para mayusculizar/minusculizar
    * de manera intercalada */
   xActiveObjectUpperLowercase.xActiveObjectConf.xCallback = vOperationUpperLowercase;
   xActiveObjectUpperLowercase.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   xActiveObjectUpperLowercase.eEventType = UART_PACKET_UPPERLOWERCASE;
   bActiveObjectRegister( &xActiveObjectUpperLowercase.xActiveObjectConf, xActiveObjectUpperLowercase.eEventType );

   /* se registra un objeto activo asociado con un evento y un callback para mayusculizar/minusculizar
    * de manera intercalada */
   xActiveObjectInvert.xActiveObjectConf.xCallback = vOperationInvert;
   xActiveObjectInvert.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   xActiveObjectInvert.eEventType = UART_PACKET_INVERT;
   bActiveObjectRegister( &xActiveObjectInvert.xActiveObjectConf, xActiveObjectInvert.eEventType );

   /* se definen los parámetros de la UART */
   xUartInstance.xUartConfig.xName = UART_USB;
   xUartInstance.xUartConfig.ulBaudRate = 115200;

   /* se inicializa uartDriver */
   if( !bUartDriverInit( &xUartInstance ) )
   {
	   /* se mantiene encendido el led RGB en rojo para indicar error */
	   gpioWrite( LEDR, ON );

	   for( ;; )
	   {}
   }
   else
   {
	   /* se mantiene encendido el led RGB en color celeste para indicar éxito */
	   gpioWrite( LEDG, ON );
   	   gpioWrite( LEDB, ON );

   	   uartWriteString( xUartInstance.xUartConfig.xName, "RTOS2 Grupo3 - TP Final\r\n");
   }

   /* creación de tareas */
   xTaskCreate( vTickTask, "Tick Task", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL );
   xTaskCreate( vDriverTask, "Driver Task", configMINIMAL_STACK_SIZE * 2, ( void * )&xUartInstance, tskIDLE_PRIORITY + 2, NULL );

   /* inicializacion del scheduler */
   vTaskStartScheduler();

   return 0;
}

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
