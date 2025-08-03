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
#define QUEUE_LED_LENGTH_		(1)
#define QUEUE_LED_ITEM_SIZE_	(sizeof(ao_led_message_t*))

/********************** internal data definition *****************************/
static GPIO_TypeDef* led_port_[] = {LED_RED_PORT, LED_GREEN_PORT,  LED_BLUE_PORT};
static uint16_t led_pin_[] = {LED_RED_PIN,  LED_GREEN_PIN, LED_BLUE_PIN };

/********************** internal functions declaration ***********************/
static void task_led(void *argument);
static void ao_led_delete(ao_led_handle_t* hao);
static void turnOnLed(ao_led_handle_t* hao);
static void turnOffLed(ao_led_handle_t* hao);
static bool ao_led_init(ao_led_handle_t* hao, ao_led_color_t color) ;
static void ao_led_queue_delete(ao_led_handle_t* hao);

/********************** internal functions definition ************************/
static void task_led(void *argument) {

	ao_led_handle_t * hao = (ao_led_handle_t*)argument;

	while (true) {

		ao_led_message_t* pmsg;

		if (pdPASS == xQueueReceive(hao->hqueue, (void*)&pmsg, portMAX_DELAY)) {

			if(AO_LED_MESSAGE_ON == pmsg->action)
				turnOnLed(hao);
			else
				turnOffLed(hao);
		}
		pmsg->process_cb(pmsg);		// avisa a UI que termino
		ao_led_delete(hao);			// y se suicida
	}
}

static void ao_led_delete(ao_led_handle_t* hao) {
	LOGGER_INFO("[LED] Elimina tarea led %d y su cola", hao->color);
	
	taskENTER_CRITICAL(); // seccion critica para que nadie inserte mensajes mientras vacio la cola
		ao_led_queue_delete(hao);
	taskEXIT_CRITICAL();
	vTaskDelete(NULL);
}

static void ao_led_queue_delete(ao_led_handle_t* hao){
	if (hao->hqueue != NULL) {
		ao_led_message_t* pmsg;

		while(pdPASS == xQueueReceive(hao->hqueue, (void*)&pmsg, 0)){
			vPortFree((void*)pmsg); // libero la memoria de posibles mensajes encolados
		}
		vQueueDelete(hao->hqueue);
		hao->hqueue = NULL;
	}
}

static void turnOnLed(ao_led_handle_t* hao) {

	HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_ON);
}

static void turnOffLed(ao_led_handle_t* hao) {

	HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_OFF);
}

static bool ao_led_init(ao_led_handle_t* hao, ao_led_color_t color) {

	if (!hao->color) hao->color = color;

	hao->hqueue = xQueueCreate(QUEUE_LED_LENGTH_, QUEUE_LED_ITEM_SIZE_);
	while(NULL == hao->hqueue) {
		LOGGER_INFO("[LED] Error! Falla creación de cola. Abortando init de LED %d", hao->color);
		return false; // salgo de ao_led_init
	}

	BaseType_t status;
	status = xTaskCreate(task_led, "task_ao_led", 128, (void*)hao, tskIDLE_PRIORITY, NULL);
	if(pdPASS != status) {
		LOGGER_INFO("[UI] Error! Falla creación de tarea. Abortando init de LED %d", hao->color);
		taskENTER_CRITICAL(); // seccion critica para que nadie mande mensajes mientras elimino
			ao_led_queue_delete(hao);
		taskEXIT_CRITICAL();
		return false; // salgo de ao_led_init
	}

	LOGGER_INFO("[LED] Crea tarea led %d", hao->color);
	return true;
}

/********************** external functions definition ************************/
bool ao_led_send(ao_led_handle_t* hao, ao_led_action_t msg, led_callback_t cbFunction) {

	if(!ao_led_init(hao, hao->color)){ // inicializa y despues manda el msg
		return pdFAIL;
	}

	BaseType_t status =  pdFAIL;
	ao_led_message_t* pmsg = (ao_led_message_t*)pvPortMalloc(sizeof(ao_led_message_t));

	if(NULL != pmsg) {

		pmsg->action = msg;
		pmsg->process_cb = cbFunction;
		status = xQueueSend(hao->hqueue, (void*)&pmsg, 0);

		if(pdPASS == status) {

			LOGGER_INFO("[LED] mensaje enviado");
		} else {

			LOGGER_INFO("[LED] mensaje no enviado");
			vPortFree((void*)pmsg);
			LOGGER_INFO("[LED] memoria liberada");
		}
	} else {
        LOGGER_INFO("[LED] Memoria insuficiente");
    }
	return (status == pdPASS);
}

/********************** end of file ******************************************/

