/*
 * ao_led.h
 *
 *  Created on: Aug 2, 2025
 *      Author: mariano
 */

#ifndef INC_AO_LED_H_
#define INC_AO_LED_H_

#include "ao_ui.h"

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef enum {

	AO_LED_MESSAGE_ON,
	AO_LED_MESSAGE_OFF,
	AO_LED_MESSAGE__N,
} ao_led_action_t;

typedef enum {

	AO_LED_COLOR_RED,
	AO_LED_COLOR_GREEN,
	AO_LED_COLOR_BLUE,
} ao_led_color_t;

typedef struct {

    ao_led_color_t color;
    QueueHandle_t hqueue;
} ao_led_handle_t;

typedef struct ao_led_message_s ao_led_message_t;

typedef void (*led_callback_t)(ao_led_message_t* pmsg); // cuando led termina, avisa a UI

struct ao_led_message_s {

	ao_led_action_t action;
	led_callback_t process_cb;
};

void ao_led_process(void);
bool ao_led_init(ao_led_handle_t* hao, ao_led_color_t color);
bool ao_led_send_event(ao_led_handle_t* hao, ao_led_action_t msg, led_callback_t cbFunction);

#endif /* INC_AO_LED_H_ */
