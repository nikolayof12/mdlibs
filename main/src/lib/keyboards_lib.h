#ifndef KEYBOARDS_LIB_H
#define KEYBOARDS_LIB_H

#include <stdint.h>

#include <EncButton.h>

/*
 * Overview
 *
 *
 * Usage:
 * For example, we have 1 encoder (D3, D4, D5 pins) and 3 buttons (D7, D8, D9 pins)
 *
 *
 * KEYBOARDS_REGISTER_ENCODER(enc_menu, 3, 4, 5);
 * KEYBOARDS_REGISTER_ENCODERS_ARRAY(my_encoders, 1);
 *
 * KEYBOARDS_REGISTER_BUTTON(accept, 7);
 * KEYBOARDS_REGISTER_BUTTON(cancel, 8);
 * KEYBOARDS_REGISTER_BUTTON(back, 9);
 * KEYBOARDS_REGISTER_BUTTONS_ARRAY(my_buttons, 3);
 *
 * KEYBOARDS_REGISTER_SERVICE_STRUCT(my_keyboard);
 *
 * uint8_t some_init_func(void)
 * {
 *	// collect all keyboard elements into one main structure:
 *	my_keyboard.buttons = my_buttons;
 *	my_keyboard.buttons[0] = accept;
 *	my_keyboard.buttons[1] = cancel;
 *	my_keyboard.buttons[2] = back;
 *	my_keyboard.buttons_count = 3;
 *	my_keyboard.encoders = my_encoders;
 *	my_keyboard.encoders[0] = enc_menu;
 *	my_keyboard.encoders_count = 1;
 *
 *
 *	keyboards_lib_init(&my_keyboard);
 *
 *	// next you can create as many 'keyboard_action' structures as you need and
 *	// dynamicalli change them in the struct button/encoder
 *	return 0;
 * }
 *
 * Somewhere in loop() function call:
 *	keyboards_lib_refresh(&my_keyboard);
 */


/*
 * Register new 'struct button' static variable to button control
 * Pin are automatically set to INPUT_PULLUP
 *
 * @name - name for new 'struct button" obj
 * @pin - pin number, where button is connected
 *
 *
 * TODO: you don't need to call 'pinMode(pin, INPUT_PULLUP)' in your setup() func????
 */
#define KEYBOARDS_REGISTER_BUTTON(name, pin)				\
	static Button (_btn_ ## name)((pin));				\
	static struct button (name) = {					\
		.obj = &(_btn_ ## name)					\
	}

/*
 * Register new array of 'struct button' objects for all your buttons
 *
 * @name - name for new array
 * @size - count of 'struct button' items in array
 */
#define KEYBOARDS_REGISTER_BUTTONS_ARRAY(name, size)			\
	static struct button (name)[(size)]

/*
 * Register new 'struct encoder' static variable to encoder control
 *
 * @name - name for new 'struct encoder' obj
 * @left - pin number, where left turn is connected
 * @right - pin number, where right turn is connected
 * @key - pin number, where key (aka button) is connected
 */
#define KEYBOARDS_REGISTER_ENCODER(name, left, right, key)		\
	static EncButton (_enc_ ## name)((left), (right), (key));	\
	static struct encoder (name) = {				\
		.obj = &(_enc_ ## name)					\
	}

/*
 * Register new array of 'struct encoder' objects for all your encoders
 *
 * @name - name for new array
 * @size - count of 'struct encoder' items in array
 */
#define KEYBOARDS_REGISTER_ENCODERS_ARRAY(name, size)			\
	static struct encoder (name)[(size)]

/*
 * Register new 'struct keyboard_service' for all your keyboard items
 *
 * @name - name for new 'struct keyboard_service' object
 */
#define KEYBOARDS_REGISTER_SERVICE_STRUCT(name)				\
	static struct keyboard_service (name) = {			\
		.encoders = NULL;					\
		.buttons = NULL;					\
	}


typedef void *(*action_func)(void *arg);

/*
 * Some action of keyboard (press btn, turn encoder...)
 *
 * call:
 *	struct keyboard_action action;
 *
 *	action.func(action.data);
 */
struct keyboard_action {
	action_func func;		/* pointer */
	void *data;
};

struct encoder {
	struct keyboard_action *left_turn;
	struct keyboard_action *left_pressed_turn;
	struct keyboard_action *right_turn;
	struct keyboard_action *right_pressed_turn;

	/* service fields */
	EncButton *obj;
};

struct button {
	struct keyboard_action *click;
	struct keyboard_action *long_click;

	/* service fields */
	Button *obj;
};

struct keyboard_service {
	struct encoder *encoders;	/* array */
	uint8_t encoders_count;

	struct button *buttons;		/* array */
	uint8_t buttons_count;
};


uint8_t keyboards_lib_init(struct keyboard_service *service);

/* return 1 if one of bts clicked, else 0 */
uint8_t keyboards_lib_refresh(struct keyboard_service *service);


#endif
