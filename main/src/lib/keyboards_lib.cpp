#include <stdint.h>

#include <EncButton.h>

#include "lib/keyboards_lib.h"


uint8_t keyboards_lib_init(struct keyboard_service *service)
{}


uint8_t _refresh_encoder(struct encoder *enc)
{
	struct keyboard_action *action;

	enc->obj->tick();

	if (enc->obj->right()) {
		if (!enc->right_turn || !enc->right_turn->func)
			return 1;

		action = enc->right_turn;
		action->func(action->data);

		return 0;
	}

	if (enc->obj->rightH()) {
		if (!enc->right_pressed_turn || !enc->right_pressed_turn->func)
			return 1;

		action = enc->right_pressed_turn;
		action->func(action->data);

		return 0;
	}

	if (enc->obj->left()) {
		if (!enc->left_turn || !enc->left_turn->func)
			return 1;

		action = enc->left_turn;
		action->func(action->data);

		return 0;
	}

	if (enc->obj->leftH()) {
		if (!enc->left_pressed_turn || !enc->left_pressed_turn->func)
			return 1;

		action = enc->left_pressed_turn;
		action->func(action->data);

		return 0;
	}

	if (enc->obj->click()) {
		if (!enc->click || !enc->click->func)
			return 1;

		action = enc->click;
		action->func(action->data);

		return 0;
	}


	return 0;
}


uint8_t _refresh_button(struct button *btn)
{
	struct keyboard_action *action;

	btn->obj->tick();

	if (btn->obj->click()) {
		if (!btn->click || !btn->click->func)
			return 1;

		action = btn->click;
		action->func(action->data);
		return 0;
	}

	if (btn->obj->hold()) {
		if (!btn->long_click || !btn->long_click->func)
			return 1;

		action = btn->long_click;
		action->func(action->data);
		return 0;
	}

	return 0;
}

uint8_t keyboards_lib_refresh(struct keyboard_service *keyboard)
{
/* TODO: add 'ret' computing */
	uint8_t ret;

	for (int i = 0; i < keyboard->encoders_count; i++)
		ret = _refresh_encoder(&keyboard->encoders[i]);

	for (int i = 0; i < keyboard->buttons_count; i++)
		ret = _refresh_button(&keyboard->buttons[i]);

	return ret;
}
