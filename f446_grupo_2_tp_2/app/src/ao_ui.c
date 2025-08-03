/*
 * ao_led.c
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

#include "ao_led.h"

/********************** macros and definitions *******************************/
#define QUEUE_LED_LENGTH_		(1)
#define QUEUE_LED_ITEM_SIZE_	(sizeof(ao_led_message_t*))

/********************** internal data definition *****************************/
static GPIO_TypeDef* led_port_[] = {LED_RED_PORT, LED_GREEN_PORT,  LED_BLUE_PORT};
static uint16_t led_pin_[] = {LED_RED_PIN,  LED_GREEN_PIN, LED_BLUE_PIN };

/********************** internal data definition *****************************/


/********************** internal functions declaration ***********************/
static void turnOnLed(ao_led_handle_t* hao);
static void turnOffLed(ao_led_handle_t* hao);
static void ao_led_delete_cola(ao_led_handle_t* hao);


void ao_led_process(void) {

	ao_led_message_t* pmsg;

	if (pdPASS == xQueueReceive(pmsg->hao->hqueue, (void*)&pmsg, portMAX_DELAY)) {

		if (AO_LED_MESSAGE_ON == pmsg->action)
			turnOnLed(pmsg->hao);
		else
			turnOffLed(pmsg->hao);
	    vPortFree(pmsg);
	    ao_led_delete_cola(pmsg->hao);
	}
}

bool ao_led_init(ao_led_handle_t* hao, ao_led_color_t color) {

	if(!hao->color)
		hao->color = color;
	hao->hqueue = xQueueCreate(QUEUE_LED_LENGTH_, QUEUE_LED_ITEM_SIZE_);

	if(NULL == hao->hqueue) {

		LOGGER_INFO("[LED] Error! Falla creación de cola. Abortando init de LED %d", hao->color);
		return false; // salgo de ao_led_init
	}
	LOGGER_INFO("[LED] Crea tarea led %d", hao->color);
	return true;
}

bool ao_led_send(ao_led_handle_t* hao, ao_led_action_t msg) {

	BaseType_t status =  pdFAIL;
	ao_led_message_t* pmsg = (ao_led_message_t*)pvPortMalloc(sizeof(ao_led_message_t));

	if(NULL != pmsg) {

		pmsg->action = msg;
		pmsg->hao = hao;
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

static void turnOnLed(ao_led_handle_t* hao) {

	HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_ON);
}

static void turnOffLed(ao_led_handle_t* hao) {

	HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_OFF);
}

static void ao_led_delete_cola(ao_led_handle_t* hao) {

	if (hao->hqueue != NULL) {

		vQueueDelete(hao->hqueue);
		hao->hqueue = NULL;
		LOGGER_INFO("[LED] Cola eliminada para LED %d", hao->color);
	}
	LOGGER_INFO("[LED] Elimina tarea led %d", hao->color);
	vTaskDelete(NULL);
}
