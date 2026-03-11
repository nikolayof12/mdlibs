#include <stdint.h>

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "display_lib.h"



void display_lib_init(struct display_service *display)
{
	if (!display->lcd)
		return;

	display->lcd->init();
	display->lcd->noBacklight();
	display->lcd->clear();
}


void display_lib_refresh(struct display_service *display)
{
/* now supported only to LCD1602
 *
 * here:
 *	- rewriting chars on screen every display->_refresh_time ms
 *	- on/off bg light (depending on display->_bg_light)
 *	- blinking (if neccessary) every display->_bg_blink_time
 *
 *	if blinking && light on, then no blinking
 */

	if (!display->lcd)
		return;

	/* rewrite chars */
	if (millis() - display->_refresh_timer_ >= display->_refresh_time) {
		/* TODO: modify this to use variable count of _cur_lines on screen */
		display->_refresh_timer_ = millis();

		display->lcd->setCursor(0, 0);
		for (int i = 0; i < lcd_lines_count; i++)
			display->lcd->print(display->_cur_lines[0][i]);

		display->lcd->setCursor(0, 1);
		for (int i = 0; i < lcd_lines_count; i++)
			display->lcd->print(display->_cur_lines[1][i]);
	}

	/* on/off bg light */
	if (display->_bg_light) { /* on */
		if (!display->_bg_light_state) {
			display->lcd->backlight();
			display->_bg_light_state = 1;
		}

		return; /* exit to dont't try do blinking */
	} else if (!display->_bg_light) { /* off */
		if (display->_bg_light_state) {
			display->lcd-> noBacklight();
			display->_bg_light_state = 0;
		}
		/* here without 'return', need to try blinking */
	}

	/* blink */
	if (display->_bg_blink &&
	    (millis() - display->_blink_timer_ >= display->_bg_blink_time)) {
		display->_blink_timer_ = millis();

		if (display->_bg_light_state)
			display->lcd->noBacklight();
		else
			display->lcd->backlight();

		display->_bg_light_state = ~display->_bg_light_state;
	}
}


void display_lib_set_next_screen(struct display_service *display)
{
}
