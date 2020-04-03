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

#ifndef _UARTDRIVER_H_
#define _UARTDRIVER_H_

/*==================[inclusions]=============================================*/

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "task.h"

#include "sapi.h"

#include "qmpool.h"
#include "operations.h"


/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define PROTOCOL_TIMEOUT	pdMS_TO_TICKS( 50 )				// timeout para transmisión y recepción
#define PACKET_SIZE			127 							// tamaño máximo de los paquetes
#define BLOCK_SIZE			( PACKET_SIZE + 1 )				// tamaño de los bloques reservados
#define POOL_TOTAL_BLOCKS	4								// catidad de bloques en el pool de memoria
#define POOL_SIZE			POOL_TOTAL_BLOCKS * BLOCK_SIZE	// tamaño total del pool

/*==================[typedef]================================================*/

typedef struct
{
	TimerHandle_t xRx;
	TimerHandle_t xTx;
} TimerTimeout_t;

typedef struct
{
	uartMap_t xName;
	uint32_t ulBaudRate;
} UartConfig_t;

typedef struct
{
	uint8_t ucRx;
	uint8_t ucTx;
} PacketLength_t;

typedef struct
{
	QueueHandle_t xRx;
	QueueHandle_t xTx;
} Queue_t;

typedef struct
{
	char *pucRx;
	char *pucTx;
} BlockPointer_t;

typedef struct
{
	char *pucPoolStorage;
	QMPool xTxPool;
} MemoryPool_t;

typedef struct
{
	UartConfig_t xUartConfig;		// datos de configuraciónd de la UART
	TimerTimeout_t xTimerTimeout;	// manejador de los timers
	PacketLength_t xPacketLength;	// datos de las longitudes los paquetes de transmisión y recepción
	Queue_t xQueue;					// manejador de las colas
	BlockPointer_t xBlockPointer;	// punteros de los paquetes de transmisión y recepción
	uint8_t ucTxCounter;			// contador de los datos a transmitir
	MemoryPool_t xMemoryPool;		// datos de la libreriá QMpool
} UartInstance_t;

/*==================[external data declaration]==============================*/

UartInstance_t xUartInstance;

/*==================[external functions declaration]=========================*/

bool_t bUartDriverInit( UartInstance_t *xUartInstance );
void vUartDriverProcessPacket( UartInstance_t *pxUartInstance );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _UARTDRIVER_H_ */
