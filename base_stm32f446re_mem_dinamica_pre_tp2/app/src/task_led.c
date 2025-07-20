/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "task_led.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_ 		(1000)

#define QUEUE_LED_LENGTH_ 		(1)
#define QUEUE_LED_ITEM_SIZE_ 	(sizeof(ao_led_message_t))

/********************** internal data declaration ****************************/
/********************** internal functions declaration ***********************/
/********************** internal data definition *****************************/

typedef enum
{
	LED_COLOR_NONE,
	LED_COLOR_RED,
	LED_COLOR_GREEN,
	LED_COLOR_BLUE,
	LED_COLOR_WHITE,
	LED_COLOR__N,
} led_color_t;

static GPIO_TypeDef *led_port_[] = {LED_RED_PORT, LED_GREEN_PORT, LED_BLUE_PORT};
static uint16_t led_pin_[] = {LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN};

/********************** external data definition *****************************/
/********************** internal functions definition ************************/
/********************** external functions definition ************************/

static void task_led(void *argument) {

	ao_led_handle_t * hao = (ao_led_handle_t*)argument;

//	LOGGER_INFO("[LED] Cola de mensajes creada: color=%d, hqueue=%p", hao->color, (void *)hao->hqueue);
	HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_OFF);

	while (true) {

	    ao_led_message_t msg;
	    if (pdPASS == xQueueReceive(hao->hqueue, &msg, portMAX_DELAY))
	    {
	      if(AO_LED_MESSAGE_ON == msg.action) {
	  		  LOGGER_INFO("[LED] LED %d: mensaje ON recibido (ID=%d)", hao->color, msg.id);
	          HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_ON);
	  		  vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
	  		  LOGGER_INFO("[LED] LED %d: se termina el tiempo de encendido (ID=%d)", hao->color, msg.id);
	  		  HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_OFF);
	      }
	    }
	}
}

void ao_led_init(ao_led_handle_t *hao, ao_led_color color) {

	hao->color = color;

	hao->hqueue = xQueueCreate(QUEUE_LED_LENGTH_, QUEUE_LED_ITEM_SIZE_);
	while (NULL == hao->hqueue) { /*error*/ }

	BaseType_t status;
	status = xTaskCreate(task_led, "task_ao_led", 128, (void *const)hao, tskIDLE_PRIORITY, NULL);
	while (pdPASS != status) { /*error*/ }
}

bool ao_led_send(ao_led_handle_t *hao, ao_led_message_t *msg) {

	//LOGGER_INFO("[LED] Enviando mensaje a cola: color=%d, hqueue=%p", hao->color, (void *)hao->hqueue);
	BaseType_t status = xQueueSend(hao->hqueue, (void *)msg, 0);

	if (status != pdPASS) {

		LOGGER_INFO("[LED] LED %d: cola llena (hqueue=%p), mensaje perdido (ID=%d)", hao->color, (void *)hao->hqueue, (int)msg->id);
	} else {

		LOGGER_INFO("[LED] LED %d: mensaje enviado (ID=%d)", hao->color, (int)msg->id);
	}
	return (status == pdPASS);
}
/********************** end of file ******************************************/
