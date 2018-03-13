/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    TAREA8_RTOS.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
/* TODO: insert other include files here. */

#define EVENT_60_SECONDS (1<<0)
#define EVENT_60_MINUTES (1<<1)
#define EVENT_60_HOURS   (1<<2)
#define EVENT_ALARM   	 (1<<3)
#define EVENT_PRINT   	 (1<<4)

EventGroupHandle_t g_time_events;

void seconds_task(void *arg)
{
	   TickType_t xLastWakeTime;

		const TickType_t xPeriod = pdMS_TO_TICKS( 1000 );
		xLastWakeTime = xTaskGetTickCount();

	    uint8_t seconds = 58;

		for(;;)
		{
			vTaskDelayUntil( &xLastWakeTime, xPeriod );
			seconds++;
			if(60 == seconds)
			{
				seconds = 0;
				xEventGroupSetBits(g_time_events,EVENT_60_SECONDS);
			}
			xEventGroupSetBits(g_time_events,EVENT_ALARM);
		}
}

void minutes_task(void *arg)
{
	uint8_t minutes = 59;
		for(;;)
		{
			xEventGroupWaitBits(g_time_events, EVENT_60_SECONDS, pdTRUE, pdTRUE, portMAX_DELAY);
	        minutes++;
			if(60 == minutes)
			{
				minutes = 0;
				xEventGroupSetBits(g_time_events,EVENT_60_MINUTES);

			}
			xEventGroupSetBits(g_time_events,EVENT_ALARM);
		}
}

void hours_task(void *arg)
{

	    uint8_t hours = 22;
		for(;;)
		{
			xEventGroupWaitBits(g_time_events, EVENT_60_MINUTES, pdTRUE, pdTRUE, portMAX_DELAY);
			hours++;
			if(24 == hours)
			{
				hours = 0;
				xEventGroupSetBits(g_time_events,EVENT_ALARM);
			}
			xEventGroupSetBits(g_time_events,EVENT_ALARM);
		}
}

void alarm_task(void *arg)
{
	for(;;)
	{
	xEventGroupWaitBits(g_time_events, EVENT_ALARM, pdTRUE, pdTRUE, portMAX_DELAY);

	xEventGroupSetBits(g_time_events,EVENT_PRINT);

	}
}

void print_task(void *arg)
{
	for(;;)
		{

		}
}

/*
 * @brief   Application entry point.
 */
int main(void)
{

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    CLOCK_EnableClock(kCLOCK_PortA);
    CLOCK_EnableClock(kCLOCK_PortC);
    g_time_events = xEventGroupCreate();

		xTaskCreate(seconds_task, "SEC task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
		xTaskCreate(minutes_task, "MIN task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);
		xTaskCreate(hours_task,   "HRS task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);
		xTaskCreate(alarm_task,   "Alarm_Task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);
		xTaskCreate(print_task,   "Print_Task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);



    vTaskStartScheduler();
    /* Enter an infinite loop, just incrementing a counter. */
    while(1)
    {

    }
    return 0;
}
