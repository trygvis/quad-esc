#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				0
#define configUSE_CO_ROUTINES			0
#define configUSE_16_BIT_TICKS			0

#define configMINIMAL_STACK_SIZE        10
#define configCPU_CLOCK_HZ				F_CPU
#define configTICK_RATE_HZ				1000
#define configMAX_PRIORITIES			2
// This is an arbitrary number for now
#define configTOTAL_HEAP_SIZE			400

#define INCLUDE_vTaskPrioritySet		0
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				0
#define INCLUDE_vTaskSuspend			0
#define INCLUDE_vTaskDelayUntil			0
#define INCLUDE_vTaskDelay				0

#endif
