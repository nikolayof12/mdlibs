#ifndef KEYBOARDS_LIB_H
#define KEYBOARDS_LIB_H

#include <stdint.h>

#include <EncButton.h>

/*
 *	Overview
 *
 *	This library provides centralized control of the following elements:
 *		- buttons
 *		- encoders
 *
 *	Each keyboard element is a data structure:
 *		- struct button
 *		- struct encoder
 *
 *	The entire keyboard is represented by a 'struct keyboard_service', which stores
 *	arrays of your encoders and buttons and other elements (arrays of structures) and
 *	variables with the value of their number.
 *
 *	The library offers you a list of macros that create global static keyboard element
 *	objects:
 *		- KEYBOARDS_REGISTER_BUTTON();
 *		- KEYBOARDS_REGISTER_ENCODER();
 *
 *
 *	Usage:
 *	For example, we have 1 encoder (D3, D4, D5 pins) and 3 buttons (D7, D8, D9 pins)
 *
 *
 * KEYBOARDS_REGISTER_ENCODER(enc_menu, 3, 4, 5);
 * struct encoder my_encoders[1];
 *
 * KEYBOARDS_REGISTER_BUTTON(accept, 7);
 * KEYBOARDS_REGISTER_BUTTON(cancel, 8);
 * KEYBOARDS_REGISTER_BUTTON(back, 9);
 * struct button my_buttons[3];
 *
 * struct keyboard_service my_keyboard;
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
 *
 *
 * You can set hardware interrupts in some pins, for example to encoder:
 *
 * void some_isr(void)
 * {
 *	enc_menu.obj->tickISR();
 * }
 *
 * void setup(void)
 * {
 *	attachInterrupt(digitalPinToInterrupt(YOUR_ENC_PIN_LEFT), some_isr, CHANGE);
 *	attachInterrupt(digitalPinToInterrupt(YOUR_ENC_PIN_RIGHT), some_isr, CHANGE);
 * }
 * NOTE:
 *	Of cource, you need keyboard_action's for every encoder turns -- compute it in other funcs
 *	Before using, check wich pins have hardware interrupts available to you.
 */


/*
 * Register new 'struct button' static variable to button control
 * Pin are automatically set to INPUT_PULLUP
 *
 * @name - name for new 'struct button" obj
 * @pin - pin number, where button is connected
 *
 * NOTE:
 *	Here, 'button' is simply a connection from GPIO @pin to GND
 */
#define KEYBOARDS_REGISTER_BUTTON(name, pin)				\
	static Button (_btn_ ## name)((pin));				\
	static struct button (name) = {					\
		.click = NULL,						\
		.long_click = NULL,					\
		.obj = &(_btn_ ## name)					\
	}


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
		.left_turn = NULL,					\
		.left_pressed_turn = NULL,				\
		.right_turn = NULL,					\
		.right_pressed_turn = NULL,				\
		.click = NULL,						\
		.obj = &(_enc_ ## name)					\
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
	struct keyboard_action *click;

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
uint8_t keyboards_lib_refresh(struct keyboard_service *keyboard);


#endif
