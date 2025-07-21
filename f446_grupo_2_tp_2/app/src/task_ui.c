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
#include "task_led.h"

/********************** macros and definitions *******************************/
#define QUEUE_LENGTH_            (1)
#define QUEUE_ITEM_SIZE_         (sizeof(msg_event_t))

/********************** internal data declaration ****************************/
/********************** internal functions declaration ***********************/
/********************** internal data definition *****************************/
typedef enum {

	UI_STATE_STANDBY,
	UI_STATE_RED,
	UI_STATE_GREEN,
	UI_STATE_BLUE,
	UI_STATE_DELETED,
} ui_state_t;

ui_state_t estado_ui = UI_STATE_DELETED;
/********************** external data definition *****************************/
ao_led_handle_t led_red, led_green, led_blue;
extern QueueHandle_t hqueue;

/********************** internal functions definition ************************/
/********************** external functions definition ************************/
static void task_ui(void *argument) {

	while (true) {

		if(estado_ui == UI_STATE_STANDBY){ // solo va a procesar nuevos mensajes si esta en SB

			msg_t* pmsg;
			LOGGER_INFO("[UI running] hqueue = %p", (void*)hqueue);
			if (pdPASS == xQueueReceive(hqueue, (void*)&pmsg, 1000)) {

				switch (pmsg->data) {

					case MSG_EVENT_BUTTON_PULSE:
						ao_led_init(&led_red, AO_LED_COLOR_RED);
						LOGGER_INFO("[UI] estado led red");
						ao_led_send(&led_red, AO_LED_MESSAGE_ON);
						estado_ui = UI_STATE_RED;
						break;
					case MSG_EVENT_BUTTON_SHORT:
						ao_led_init(&led_green, AO_LED_COLOR_GREEN);
						LOGGER_INFO("[UI] estado led green");
						ao_led_send(&led_green, AO_LED_MESSAGE_ON);
						estado_ui = UI_STATE_GREEN;
						break;
					case MSG_EVENT_BUTTON_LONG:
						ao_led_init(&led_blue, AO_LED_COLOR_BLUE);
						LOGGER_INFO("[UI] estado led blue");
						ao_led_send(&led_blue, AO_LED_MESSAGE_ON);
						estado_ui = UI_STATE_BLUE;
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
}

void ao_ui_init(void)
{
	// agrego logica para que se cree la tarea solo si no hay una corriendo
	if(estado_ui == UI_STATE_DELETED) {
		LOGGER_INFO("[UI] Se crea la tarea UI");
		BaseType_t status;
		status = xTaskCreate(task_ui, "task_ao_ui", 128, NULL, tskIDLE_PRIORITY, NULL);
		while (pdPASS != status) { }
	}
	estado_ui = UI_STATE_STANDBY;
}

bool ao_ui_send_event(msg_event_t msg) {

	BaseType_t status =  pdFAIL;
	msg_t* pmsg = (msg_t*)pvPortMalloc(sizeof(msg_t));

	if(NULL != pmsg) {

		LOGGER_INFO("[UI] memoria alocada: %d", sizeof(msg_t));
		pmsg->size = sizeof(msg_t);
		pmsg->msg_entregado = false;
		pmsg->data = msg;
		LOGGER_INFO("[UI] hqueue = %p", (void*)hqueue);
		status = xQueueSend(hqueue, (void*)&pmsg, 0);

		if(pdPASS == status) {

			LOGGER_INFO("[UI] mensaje enviado");
		} else {

			LOGGER_INFO("[UI] mensaje no enviado");
			vPortFree((void*)pmsg);
			LOGGER_INFO("[UI] memoria liberada");
		}
	} else {

		LOGGER_INFO("[BUTTON] memoria insuficiente");
	}
	return (status == pdPASS);
}

void ao_ui_delete(void) {

	if(estado_ui != UI_STATE_STANDBY){
	  LOGGER_INFO("[UI] Se intenta borrar UI procesando!!");
	} else {
	  LOGGER_INFO("[UI] Elimino tarea ui");
	  estado_ui = UI_STATE_DELETED;
	  vTaskDelete(NULL);
	}
}

void ao_ui_callback(void){
	// cuando el led termina de procesar se llama este callback para volver la UI a SB (y liberar la mem del msg luego)
	estado_ui = UI_STATE_STANDBY;
	LOGGER_INFO("[UI] Vuelve a SB");
}
/********************** end of file ******************************************/
