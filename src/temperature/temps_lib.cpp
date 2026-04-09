#include <stdint.h>

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "lib/temps_lib.h"


/*
 * Setup individual sensor
 * NOTE: this is a DS18B20-specific function
 *
 * @sensor - ptr to struct temp_sensor, here will be setted values
 * @obj - ptr to DallasTemperature obj, which can control multiple sensors
 * @res - resolution, needed to calculate temp update time
 * @index - index of sensor in 1-Wire
 * @devices_count - count of expected devices on Wire
 *
 * return 0 if initialization is successful, else *_lib_ec error code
 */
uint8_t temps_lib_init_sensor(struct temp_sensor *sensor,
			      DallasTemperature *obj,
			      enum accuracy res,
			      uint8_t index,
			      uint8_t devices_count)
{
/* TODO: here setup the addresses of every sensor, etc... */
	uint32_t interval = 0;

	if (!sensor)
		return struct_not_found_lib_ec;

	if (!obj)
		return dt_obj_not_found_lib_ec;

	if (obj->getDeviceCount() != devices_count)
		return device_not_found_lib_ec;


	obj->getAddress(sensor->address, index);
	obj->setResolution(sensor->address, res);

	switch (res) {
	case simple:
		interval = DS18B20_9_BIT_TIME;
	case standard:
		interval = DS18B20_10_BIT_TIME;
	case advanced:
		interval = DS18B20_11_BIT_TIME;
	case special:
	default:
		interval = DS18B20_12_BIT_TIME;
	}

	sensor->req_interval = interval;
	sensor->obj = obj;
	sensor->is_enable = true;

	return 0;
}


uint8_t _refresh_sensor(struct temp_sensor *sensor)
{
/* TODO: handle possible read errors */
	uint8_t ret = 0;

	/* exit if sensor disable */
	if (!sensor->is_enable)
		return ret;

	if (sensor->_read_timer &&
	    (millis() - sensor->_read_timer >= sensor->req_interval)) {
		/* temp ready, read it, set timer to 0 */
		fl_t new_temp;

		/* 10 - to convert float to fl_t: (77.7 * 10) = 777 that good */
		new_temp = (fl_t)(sensor->obj->getTempC(sensor->address) * 10);
		if (new_temp != sensor->cur_temp) {
			sensor->prev_temp = sensor->cur_temp;
			sensor->changes_timer = millis();
		}

		sensor->cur_temp = new_temp;
		sensor->_read_timer = 0;	/* neccessary */
	}

	if (!sensor->_read_timer) {
		/* request temp, set timer */
		sensor->obj->requestTemperaturesByAddress(sensor->address);
		sensor->_read_timer = millis();
	}

	return ret;
}


/* Here only read/set temperatures, without comparing tar/cur */
uint8_t temps_lib_refresh(struct temps_service *service)
{
/* TODO: here read the temps, update the timers, etc... */
	struct temp_sensor *sensor;
	uint8_t ret = 0;

	for (int i = 0; i < service->sensors_count; i++) {
		sensor = &(service->sensors[i]);
		ret = _refresh_sensor(sensor);
		if (ret)
			sensor->errors++;
	}

	return ret;
}


/*
 * Convert fl_t to str
 * Right-aligned buffer, replace all buffer items to ' ' before result
 *
 * @num - number for convert (10000 > num > 10)
 * @buff[5] - str for result, it is assumed that it will fit in it
 * @is_float - flag, 0 - if num is simple number, !0 - if num is a float num
 *
 * char *res = "aaaaa";
 * temps_lib_convert(333, res, 1);
 * -> res == " 33.3"
 * temps_lib_convert(333, res, 0);
 * -> res == "333aa"
 *
 *  max:
 *	temps_lib_convert(999, res, 1)
 *	-> "999.9"
 *	temps_lib_convert(999, res, 0)
 *	-> "999  "
 *
 *  err:
 *	temps_lib_convert(10000, res, 1)
 *	-> "aeraa"
 *	temps_lib_convert(10000, res, 0)
 *	-> "aeraa"
 */
uint8_t *temps_lib_convert(fl_t num, uint8_t buff[5], uint8_t is_float)
{
	uint32_t tmp;
	int data;

	if ((num < 10) || (num >= 10000)) {
		buff[1] = 'e';
		buff[2] = 'r';

		return NULL;
	}

	if (is_float) {
		buff[0] = ' ';
		buff[1] = ' ';
		buff[2] = ' ';
		buff[3] = ' ';
		buff[4] = ' ';

		buff[4] = (num % 10) + '0';
		num /= 10;
		buff[3] = '.';

		tmp = num % 10;
		num /= 10;
		buff[2] = tmp + '0';

		if (!num)
			return buff + 2;

		tmp = num % 10;
		num /= 10;
		buff[1] = tmp + '0';

		if (!num)
			return buff + 1;

		tmp = num % 10;
		num /= 10;
		buff[0] = tmp + '0';

		return buff;
	}

	/* so as num must be < 10000 -> max is 999.9, if no float, then just 999
	 * that 3 symbols, because last [3] and [4] don't change
	 */
	buff[0] = ' ';
	buff[1] = ' ';
	buff[2] = ' ';

	num /= 10;

	if (!num)
		return buff + 3;

	buff[2] = num % 10 + '0';
	num /= 10;

	if (!num)
		return buff + 2;

	buff[1] = num % 10 + '0';
	num /= 10;

	if (!num)
		return buff + 1;

	buff[0] = num % 10 + '0';
	num /= 10;

	return buff;
}
