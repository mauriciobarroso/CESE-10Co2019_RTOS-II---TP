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

ActiveObjectConf_t xActiveObjectLowercase;
ActiveObjectConf_t xActiveObjectUppercase;
ActiveObjectConf_t xActiveObjectUpperLowercase;
ActiveObjectConf_t xActiveObjectError;

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]=========================*/

int main(void)
{
   /* se inicializa la EDU-CIAA */
   boardConfig();

   /* se crea registra un objeto activo asociado con un evento y un callback para minusculizar */
   xActiveObjectLowercase.xCallback = vOperationLowercase;
   xActiveObjectLowercase.uxPriority = tskIDLE_PRIORITY + 3;
   bActiveObjectRegister( UART_PACKET_LOWERCASE, &xActiveObjectLowercase );

   /* se crea registra un objeto activo asociado con un evento y un callback para mayusculizar */
   xActiveObjectUppercase.xCallback = vOperationUppercase;
   xActiveObjectUppercase.uxPriority = tskIDLE_PRIORITY + 3;
   bActiveObjectRegister( UART_PACKET_UPPERCASE, &xActiveObjectUppercase );

   /* se crea registra un objeto activo asociado con un evento y un callback para mayusculizar/minusculizar
    * de manera intercalada */
   xActiveObjectUpperLowercase.xCallback = vOperationUpperLowercase;
   xActiveObjectUpperLowercase.uxPriority = tskIDLE_PRIORITY + 3;
   bActiveObjectRegister( UART_PACKET_UPPERLOWERCASE, &xActiveObjectUpperLowercase );

   /* se crea registra un objeto activo asociado con un evento y un callback para manejar eventos no válidos */
   xActiveObjectError.xCallback = vOperationError;
   xActiveObjectError.uxPriority = tskIDLE_PRIORITY + 3;
   bActiveObjectRegister( UART_PACKET_ERROR, &xActiveObjectError );

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
