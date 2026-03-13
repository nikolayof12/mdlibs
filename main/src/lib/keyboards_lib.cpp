#include <stdint.h>

#include <EncButton.h>

#include "lib/keyboards_lib.h"


uint8_t keyboards_lib_init(struct keyboard_service *service)
{}


uint8_t _refresh_encoder(struct encoder *enc)
{}


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

uint8_t keyboards_lib_refresh(struct keyboard_service *service)
{
}
