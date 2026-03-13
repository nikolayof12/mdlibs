#ifndef KEYBOARDS_LIB_H
#define KEYBOARDS_LIB_H

#include <stdint.h>

#include <EncButton.h>

/*
 * Overview
 */


/*
 * Register new 'struct button' static variable to button control
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


typedef void *(*action_func)(void *arg);

struct encoder {
	action_func left_action_func;
	action_func left_pressed_action_func;
	action_func right_action_func;
	action_func right_pressed_action_func;

	/* service fields */
	EncButton *obj;
};

struct button {
	action_func press_action;
	action_func long_press_action;

	/* service fields */
	Button *obj;
};

struct keyboard_service {
	struct encoder *encoders;	/* array */
	struct button *buttons;		/* array */
};


void keyboard_init(struct keyboard_service *service);

/* return 1 if one of bts clicked, else 0 */
uint8_t keyboards_lib_refresh(struct keyboard_service *service);


#endif
