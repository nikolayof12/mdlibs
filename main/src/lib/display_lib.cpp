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
 *	if blinking && light off, then do blinking
 */

	if (!display->lcd)
		return;

	/* refresh lines */
	if (millis() - display->_refresh_timer_ >= display->_refresh_time) {
		display->_refresh_timer_ = millis();

		/* TODO: need to test */
		for (int l = 0; l < display->_lcd_lines_count; l++) {
			display->lcd->setCursor(0, l);

			for (int c = 0; c < display->_lcd_line_length; c++)
				display->lcd->print((char) display->_cur_lines[0][c]);
		}
	}

	/* on/off bg light */
	if (display->_bg_light) { /* target - on */
		if (!display->_bg_light_state) {
			display->lcd->backlight();
			display->_bg_light_state = 1;
		}

		return; /* exit to dont't try do blinking */
	} else if (!display->_bg_light) { /* target - off */
		if (display->_bg_light_state && !display->_bg_blink) {
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
