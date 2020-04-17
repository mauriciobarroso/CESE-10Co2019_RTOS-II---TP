
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

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]=========================*/

int main(void)
{
   /* se inicializa la EDU-CIAA */
   boardConfig();

   /* inicialización de los objetos activos */
   xActiveObject[ 0 ].bAlive = 0;
   xActiveObject[ 0 ].xCallback = vOperationLowercase;
   xActiveObject[ 0 ].uxPriority = tskIDLE_PRIORITY + 3;

   xActiveObject[ 1 ].bAlive = 0;
   xActiveObject[ 1 ].xCallback = vOperationUppercase;
   xActiveObject[ 1 ].uxPriority = tskIDLE_PRIORITY + 3;

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
