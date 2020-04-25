/*
 * uartDriver.h
 *
 * Created on: Apr 9, 2020
 * Author: Grupo 3
 *
 */

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
#include "events.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define PROTOCOL_TIMEOUT	pdMS_TO_TICKS( 50 )				// timeout para transmisión y recepción
#define PACKET_SIZE			64	 							// tamaño máximo de los paquetes
#define BLOCK_SIZE			PACKET_SIZE						// tamaño de los bloques reservados
#define POOL_TOTAL_BLOCKS	20								// cantidad de bloques en el pool de memoria
#define POOL_SIZE			POOL_TOTAL_BLOCKS * BLOCK_SIZE	// tamaño total del pool

/*==================[typedef]================================================*/

typedef struct
{
	uartMap_t xName;
	uint32_t ulBaudRate;
} UartConfig_t;

typedef struct
{
	char *pucBlock;
	uint8_t ucLength;
} UartPacket_t;

typedef struct
{
	TimerHandle_t xRx;
	TimerHandle_t xTx;
} TimerTimeout_t;

typedef struct
{
	QueueHandle_t xRx;
	QueueHandle_t xTx;
} Queue_t;

typedef struct
{
	char *pucPoolStorage;
	QMPool xTxPool;
} MemoryPool_t;

typedef struct
{
	UartConfig_t xUartConfig;		// datos de configuraciónd de la UART
	UartPacket_t xRxPacket;			// paquete de recepción de la UART
	UartPacket_t xTxPacket;			// paquete de transmisión de la UART
	TimerTimeout_t xTimerTimeout;	// manejador de los timers
	Queue_t xQueue;					// manejador de las colas
	uint8_t ucTxCounter;			// contador de los datos a transmitir
	MemoryPool_t xMemoryPool;		// datos de la libreria QMpool
} UartInstance_t;

typedef struct
{
	eEventType_t EventType;			// tipo de evento
	UartPacket_t xPacket;			// paquete del evento
	QueueHandle_t xReceptionQueue;	// cola de recepción de eventos
} UartDriverEvent_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief Función para inicializar lso parámetros del driver
 *
 * @param pxUartInstance[in] Puntero a la configuración del driver
 *
 * @return
 * 		- TRUE Éxito
 * 		- FALSE Fallo al inicializar los parámetros (queue, timers o memory pool)
 */
bool_t bUartDriverInit( UartInstance_t *pxUartInstance );

/**
 * @brief Función para procesar los paquetes provenientes de la capa anterior
 *
 * @param pxUartInstance[in] Puntero a la configuración del driver
 *
 */
void vUartDriverProcessPacket( UartInstance_t *pxUartInstance );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/

#endif /* #ifndef _UARTDRIVER_H_ */
