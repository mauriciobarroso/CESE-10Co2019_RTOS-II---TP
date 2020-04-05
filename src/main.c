
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

#include "userTasks.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[external data declaration]==============================*/

UartInstance_t xUartInstance;

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]=========================*/

int main(void)
{
   /* se inicializa la EDU-CIAA */
   boardConfig();
   /* se definen los parámetros de la UART */
   xUartInstance.xUartConfig.xName = UART_USB;
   xUartInstance.xUartConfig.ulBaudRate = 115200;
   /* se inicializa uartDriver */
   if( !bUartDriverInit( &xUartInstance ) )
   {
	   gpioWrite( LEDR, ON );

	   for( ;; )
	   {}
   }
   else
	   gpioWrite( LEDG, ON );
   /* creación de tareas */
   xTaskCreate( vTickTask, "Tick Task", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL );
   xTaskCreate( vDriverTask, "Tick Task", configMINIMAL_STACK_SIZE * 2, ( void * )&xUartInstance, tskIDLE_PRIORITY + 2, NULL );
   /* inicializacion del scheduler */
   vTaskStartScheduler();

   return 0;
}

/*==================[internal functions definition]==========================*/

/*==================[end of file]============================================*/
