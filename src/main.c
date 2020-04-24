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

UartInstance_t xUartInstance;

ActiveObject_t xActiveObjectLowercase;
ActiveObject_t xActiveObjectUppercase;
ActiveObject_t xActiveObjectUpperLowercase;
ActiveObject_t xActiveObjectError;

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]=========================*/

int main(void)
{
   /* se inicializa la EDU-CIAA */
   boardConfig();

   /* se crea registra un objeto activo asociado con un evento y un callback para minusculizar */
   xActiveObjectLowercase.xActiveObjectConf.xCallback = vOperationLowercase;
   xActiveObjectLowercase.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   xActiveObjectLowercase.eEventType = UART_PACKET_LOWERCASE;
   bActiveObjectRegister( &xActiveObjectLowercase.xActiveObjectConf, xActiveObjectLowercase.eEventType );

   /* se crea registra un objeto activo asociado con un evento y un callback para mayusculizar */
   xActiveObjectUppercase.xActiveObjectConf.xCallback = vOperationUppercase;
   xActiveObjectUppercase.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   xActiveObjectUppercase.eEventType = UART_PACKET_UPPERCASE;
   bActiveObjectRegister( &xActiveObjectUppercase.xActiveObjectConf, xActiveObjectUppercase.eEventType );

   /* se crea registra un objeto activo asociado con un evento y un callback para mayusculizar/minusculizar
    * de manera intercalada */
   //xActiveObjectUpperLowercase.xActiveObjectConf.xCallback = vOperationUpperLowercase;
   //xActiveObjectUpperLowercase.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   //xActiveObjectUpperLowercase.eEventType = UART_PACKET_UPPERLOWERCASE;
   //bActiveObjectRegister( &xActiveObjectUpperLowercase.xActiveObjectConf, xActiveObjectUpperLowercase.eEventType );

   /* se crea registra un objeto activo asociado con un evento y un callback para manejar eventos no válidos */
   ////xActiveObjectError.xActiveObjectConf.xCallback = vOperationError;
   //xActiveObjectError.xActiveObjectConf.uxPriority = tskIDLE_PRIORITY + 3;
   //xActiveObjectError.eEventType = UART_PACKET_ERROR;
   //bActiveObjectRegister( &xActiveObjectError.xActiveObjectConf, xActiveObjectError.eEventType );

   /* se definen los parámetros de la UART */
   xUartInstance.xUartConfig.xName = UART_USB;
   xUartInstance.xUartConfig.ulBaudRate = 115200;

   /* se inicializa uartDriver */
   if( !bUartDriverInit( &xUartInstance ) )
   {
	   /* mantiene encendido el led rojo para indicar error */
	   gpioWrite( LEDR, ON );

	   for( ;; )
	   {}
   }
   else
	   /* mantiene encendido el led verde para indicar exito */
	   gpioWrite( LEDG, ON );

   /* creación de tareas */
   xTaskCreate( vTickTask, "Tick Task", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL );
   xTaskCreate( vDriverTask, "Driver Task", configMINIMAL_STACK_SIZE * 2, ( void * )&xUartInstance, tskIDLE_PRIORITY + 2, NULL );
   /* inicializacion del scheduler */
   vTaskStartScheduler();

   return 0;
}

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
