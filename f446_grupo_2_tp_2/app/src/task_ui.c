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

#include "task_ui.h"
#include "task_button.h"

/********************** macros and definitions *******************************/
#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(msg_t*))

typedef enum {

	UI_STATE_STANDBY,
	UI_STATE_RED,
	UI_STATE_GREEN,
	UI_STATE_BLUE,
	UI_STATE__N,
} ui_state_t;

/********************** internal data definition *****************************/
static ui_state_t estado_ui = UI_STATE__N;
static bool ui_running;
static ao_led_handle_t led_red, led_green, led_blue;
static QueueHandle_t hqueue;

static void task_ui(void *argument) {

	while(true) {

		msg_t* pmsg;

		if(pdPASS == xQueueReceive(hqueue, (void*)&pmsg, 1000)) {

			ao_led_init(&led_red, AO_LED_COLOR_RED);
			ao_led_init(&led_green, AO_LED_COLOR_GREEN);
			ao_led_init(&led_blue, AO_LED_COLOR_BLUE);

			switch(pmsg->data) {

				case MSG_EVENT_BUTTON_PULSE:
					ao_led_send(&led_green, AO_LED_MESSAGE_OFF);
					ao_led_send(&led_blue, AO_LED_MESSAGE_OFF);
					ao_led_send(&led_red, AO_LED_MESSAGE_ON);
					estado_ui = UI_STATE_RED;
					pmsg->process_cb(pmsg);
					LOGGER_INFO("[UI] Estado RED");
					break;
				case MSG_EVENT_BUTTON_SHORT:
					ao_led_send(&led_red, AO_LED_MESSAGE_OFF);
					ao_led_send(&led_blue, AO_LED_MESSAGE_OFF);
					ao_led_send(&led_green, AO_LED_MESSAGE_ON);
					estado_ui = UI_STATE_GREEN;
					LOGGER_INFO("[UI] Estado GREEN");
					pmsg->process_cb(pmsg);
					break;
				case MSG_EVENT_BUTTON_LONG:
					ao_led_send(&led_green, AO_LED_MESSAGE_OFF);
					ao_led_send(&led_red, AO_LED_MESSAGE_OFF);
					ao_led_send(&led_blue, AO_LED_MESSAGE_ON);
					estado_ui = UI_STATE_BLUE;
					LOGGER_INFO("[UI] Estado BLUE");
					pmsg->process_cb(pmsg);
					break;
				default:
					break;
			}
		} else {

			// la tarea UI debe vivir mientras hay mensajes sin procesar en la cola
			// cuando se acaban los mensajes encolados, se suicida
			ao_ui_delete();
		}
	}
}

void ao_ui_init(void) {

	// agrego logica para que se cree la tarea solo si no hay una corriendo
	if(!ui_running) {

		LOGGER_INFO("[UI] Se crea la tarea UI");
		hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);

		while(NULL == hqueue) {/*error*/}
		LOGGER_INFO("[UI] hqueue = %p", (void*)hqueue);
		BaseType_t status;
		status = xTaskCreate(task_ui, "task_ao_ui", 128, NULL, tskIDLE_PRIORITY, NULL);

		while(pdPASS != status) {/*error*/}
		if(estado_ui == UI_STATE__N) estado_ui = UI_STATE_STANDBY;
	}
	ui_running = true;
}

bool ao_ui_send_event(msg_event_t msg) {

	BaseType_t status =  pdFAIL;
	msg_t* pmsg = (msg_t*)pvPortMalloc(sizeof(msg_t));

	if(NULL != pmsg) {

		LOGGER_INFO("[UI] memoria alocada: %d", sizeof(msg_t));
		pmsg->size = sizeof(msg_t);
		pmsg->data = msg;
		pmsg->process_cb = button_callback;
		status = xQueueSend(hqueue, (void*)&pmsg, 0);

		if(pdPASS == status) {

			LOGGER_INFO("[UI] mensaje enviado");
		} else {

			LOGGER_INFO("[UI] mensaje no enviado");
			vPortFree((void*)pmsg);
			LOGGER_INFO("[UI] memoria liberada");
		}
	} else {

		LOGGER_INFO("[UI] memoria insuficiente");
	}
	return status;
}

void ao_ui_delete(void) {

	  LOGGER_INFO("[UI] Elimino tarea ui"); // ahora se elimina en cualquier estado
	  ui_running = false;
	  vTaskDelete(NULL);
}

void ao_ui_callback(ao_led_message_t* pmsg) {

	// cuando el led termina de procesar se llama este callback para volver la UI a SB y liberar la mem del msg
	vPortFree((void*)pmsg);
	LOGGER_INFO("[UI] Callback: memoria liberada");
}
/********************** end of file ******************************************/
