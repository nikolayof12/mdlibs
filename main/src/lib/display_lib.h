#ifndef DISPLAY_LIB_H
#define DISPLAY_LIB_H

#include <stdint.h>

#include <LiquidCrystal_I2C.h>


/**		OVERVIEW
 * You must define:				Default values:
 *	#define DISPLAY_USE_LCD_1602		just define
 *
 *
 * Use macroses with this lib:
 *	DISPLAY_REGISTER_NEW_LCD(my_lcd, 0x27, 16, 2);
 *	DISPLAY_REGISTER_NEW_LCD_LINES(my_lines, 4);
 *
 * Usage:
 *	#define DISPLAY_USE_LCD_1602
 *
 *	DISPLAY_REGISTER_NEW_LCD(my_lcd, 0x27, 16, 2);
 *	DISPLAY_REGISTER_NEW_LCD_LINES(my_lines, 4);
 *	...
 *	void my_display_init(struct display_service *display)
 *	{
 *		DISPLAY_INIT_SERVICE_STRUCT(*display);
 *
 *		display->lcd = &my_lcd;
 *		display->lines = my_lines;
 *
 *		display_lib_init(display);
 *		...
 *	}
 */

/* Today, supported LCD1602 only */
#ifndef DISPLAY_USE_LCD_1602
#define DISPLAY_USE_LCD_1602
#endif


#ifdef DISPLAY_USE_LCD_1602
enum { lcd_line_length = 16, lcd_lines_count = 2};
typedef uint8_t row[lcd_line_length];	/* one line on display */
#endif /* DISPLAY_USE_LCD_1602 */

/**
 * Regiter new lcd display as LiquidCrystal_I2C object with your parameters
 * you can call this object by 'name' name
 *
 * usage:
 *	DISPLAY_REGISTER_NEW_LCD(first_lcd, 0x27, 16, 2);
 *	struct display_service display;
 *
 *	display.lcd = first_lcd;
 *	...
 *	first_lcd.init()
 *	first_lcd.clear()
 *
 * call WITH ';'
 */
#define DISPLAY_REGISTER_NEW_LCD(name, addr, width, height)	\
	static LiquidCrystal_I2C (name)((addr), (width), (height))

/**
 * Register new static array as buff to LCD display lines
 *
 * usage:
 *	DISPLAY_REGISTER_NEW_LCD_LINES(my_lines, 4);
 *
 *	struct display_service display;
 *	display.lines = my_lines;
 *	printf(display.lines[0]);
 *
 * call WITH ';'
 */
#define DISPLAY_REGISTER_NEW_LCD_LINES(name, count)		\
	static row (name)[(count)]

/**
 * Initialize the service struct to default values
 * Call it after create the variable
 *
 * usage:
 *	struct display_service *display = malloc(...); // or other
 *	DISPLAY_INIT_SERVICE_STRUCT(*display);
 *	...
 *
 * call WITH ';'
 */
#define DISPLAY_INIT_SERVICE_STRUCT(display)			\
	do {							\
		(display).lcd = NULL;				\
		(display).lines = NULL;				\
		(display)._cur_lines[0] = (display).lines[0];	\
		(display)._cur_lines[1] = (display).lines[1];	\
		(display)._bg_light = 0;	 /* disable */	\
		(display)._bg_light_state = 0;	 /* disable */	\
		(display)._bg_blink = 0;	 /* disable */	\
		(display)._blink_time = 500;	 /* every ms */ \
		(display)._refresh_time = 500;	 /* every ms */	\
	} while (0)

struct display_service {
	/* use macroses to gen this fields */
	LiquidCrystal_I2C *lcd;
	row *lines;

	/* TODO:can be replaced with bit fields */
	uint8_t *_cur_lines[lcd_lines_count];	/* lines that are now on disp */
	uint8_t _bg_light;		/* 0 - off, 1 - on; to manual switching */
	uint8_t _bg_light_state;	/* 0 - off, 1 - on; to read current state */
	uint8_t _bg_blink;		/* 0 - no blinnk, 1 - blink every _bg_blink_time ms */

	uint16_t _blink_time;		/* ms, to bg blink */
	uint32_t _blink_timer_;		/* internal timer */
	uint16_t _refresh_time;		/* ms, to update text */
	uint32_t _refresh_timer_;	/* internal timer */
};

void display_lib_init(struct display_service *display);
void display_lib_refresh(struct display_service *display);
void display_lib_set_next_screen(struct display_service *display);


/* TODO: add here 'static inline' funcs to get/set valuses to *display */

static inline uint8_t display_lib_get_bg_light_state(struct display_service *display)
{
	/* 0 - off, 1 - on */
	return display->_bg_light_state;
}

#endif
