#ifndef KEYBOARDS_LIB_H
#define KEYBOARDS_LIB_H

#include <stdint.h>

/*
 * Overview
 */


struct keyboard_service {
	uint8_t *buttons;
};

#if 0
struct btn {
	uint8_t pin;
	uint8_t state;
};

void keyboard_init(struct keyboard_service *service);

/* return 1 if one of bts clicked, else 0 */
uint8_t keyboard_refresh(struct keyboard_service *service);
#endif

#endif
