/*
 * ao_ui.c
 *
 *  Created on: Aug 2, 2025
 *      Author: mariano
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao_ui.h"

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
static QueueHandle_t hqueue;
static ui_state_t estado_ui = UI_STATE__N;

/********************** internal functions declaration ***********************/


void ao_ui_process(void) {

	msg_t* pmsg;


	while(pdPASS == xQueueReceive(hqueue, (void*) &pmsg, 1000)) {

		bool msgSent = pdFAIL;

		switch(pmsg->data) {

			case MSG_EVENT_BUTTON_PULSE:
				msgSent = pdFAIL;
/*				if(UI_STATE_STANDBY == estado_ui)
					msgSent = pdPASS; // no requiere manda ningun otro mensaje
				if(UI_STATE_GREEN == estado_ui)
					msgSent = ao_led_send(&led_green, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(UI_STATE_BLUE == estado_ui)
					msgSent = ao_led_send(&led_blue, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(msgSent && ao_led_send(&led_red,  AO_LED_MESSAGE_ON, ao_ui_callback)) {
*/					estado_ui = UI_STATE_RED;
					LOGGER_INFO("[UI] Estado RED");
//				}
				pmsg->process_cb(pmsg);
				break;
			case MSG_EVENT_BUTTON_SHORT:
/*				if(UI_STATE_STANDBY == estado_ui)
					msgSent = pdPASS; // no requiere manda ningun otro mensaje
				if(UI_STATE_RED == estado_ui)
					msgSent = ao_led_send(&led_red, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(UI_STATE_BLUE == estado_ui)
					msgSent = ao_led_send(&led_blue, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(msgSent && ao_led_send(&led_green, AO_LED_MESSAGE_ON, ao_ui_callback)){
*/					estado_ui = UI_STATE_GREEN;
					LOGGER_INFO("[UI] Estado GREEN");
//				}
				pmsg->process_cb(pmsg);
				break;
			case MSG_EVENT_BUTTON_LONG:
/*				if(UI_STATE_STANDBY == estado_ui)
					msgSent = pdPASS; // no requiere manda ningun otro mensaje
				if(UI_STATE_RED == estado_ui)
					msgSent = ao_led_send(&led_red, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(UI_STATE_GREEN == estado_ui)
					msgSent = ao_led_send(&led_green, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(msgSent && ao_led_send(&led_blue, AO_LED_MESSAGE_ON, ao_ui_callback)){
*/					estado_ui = UI_STATE_BLUE;
					LOGGER_INFO("[UI] Estado BLUE");
//				}
				pmsg->process_cb(pmsg);
				break;
			default:
				break;
		}

		// Aviso al callback del remitente (button)
		if (pmsg->process_cb) {
			pmsg->process_cb(pmsg);
		}
	}
}

bool ao_ui_init(void) {

	hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	if(NULL == hqueue) {
		LOGGER_INFO("[UI] Error! Falla creación de cola. Abortando init de UI.");
		return false; // salgo de ao_ui_init
	}

	if(estado_ui == UI_STATE__N) estado_ui = UI_STATE_STANDBY;

	LOGGER_INFO("[UI] Crea tarea UI");
	return true;
}

bool ao_ui_send_event(msg_event_t event) {

	if(NULL == hqueue)
		return false;

	msg_t* pmsg = pvPortMalloc(sizeof(msg_t));

	if(!pmsg)
		return false;

	pmsg->size = sizeof(msg_t);
	pmsg->data = event;
	BaseType_t result = xQueueSend(hqueue, &pmsg, 0);

	if(pdPASS != result) {

		vPortFree(pmsg);
		return false;
	}
	return true;
}


