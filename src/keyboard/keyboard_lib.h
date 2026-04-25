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
 * // create global variables
 * static struct encoder my_encoders[1];
 * static struct button my_buttons[3];
 *
 * static struct keyboard_service my_keyboard;
 *
 * uint8_t some_init_func(void)
 * {
 *	KEYBOARDS_REGISTER_BUTTON(accept, 7, my_buttons[0]);
 *	KEYBOARDS_REGISTER_BUTTON(cancel, 8, my_buttons[1]);
 *	KEYBOARDS_REGISTER_BUTTON(back, 9, my_buttons[2]);
 *
 *	KEYBOARDS_REGISTER_ENCODER(enc_menu, 3, 4, 5, my_encoders[0]);
 *
 *	// collect all keyboard elements into one main structure:
 *	my_keyboard.buttons = my_buttons;
 *	my_keyboard.buttons_count = 3;
 *	my_keyboard.encoders = my_encoders;
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
 * Register new object to button control
 * Pin are automatically set to INPUT_PULLUP
 * Call this macro from your setup() function. Different buttons require
 * different @name arguments.
 *
 * @name - just a unique string
 * @pin - pin number, where button is connected
 * @data - field of 'struct button' type; the values of fields of this struct
 *		will be changed
 *
 * NOTE:
 *	Here, 'button' is simply a connection from GPIO @pin to GND
 */
#define KEYBOARDS_REGISTER_BUTTON(name, pin, data)			\
	do {								\
		static Button (_btn_ ## name)((pin));			\
		(data).click = NULL;					\
		(data).long_click = NULL;				\
		(data).obj = &(_btn_ ## name);				\
	} while (0)


/*
 * Register new object to encoder control
 * Call this macro from your setup() function. Different encoders require
 * different @name arguments.
 *
 * @name - just a unique string
 * @left - pin number, where left turn is connected
 * @right - pin number, where right turn is connected
 * @key - pin number, where key (aka button) is connected
 * @data - field of 'struct encoder' type; the values of fields of this struct
 *		will be changed
 */
#define KEYBOARDS_REGISTER_ENCODER(name, left, right, key, data)		\
	do {									\
		static EncButton (_enc_ ## name)((left), (right), (key));	\
		(data).left_turn = NULL;					\
		(data).left_pressed_turn = NULL;				\
		(data).right_turn = NULL;					\
		(data).right_pressed_turn = NULL;				\
		(data).click = NULL;						\
		(data).obj = &(_enc_ ## name);					\
	} while (0)


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

/* return 0 if all objects processed, else not 0 */
uint8_t keyboards_lib_refresh(struct keyboard_service *keyboard);


#endif
