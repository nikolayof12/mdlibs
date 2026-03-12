#include <stdint.h>

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "lib/temps_lib.h"


/*
 * Setup individual sensor
 *
 * @sensor - ptr to struct temp_sensor, here will be setted values
 * @obj - ptr to DallasTemperature obj, which can control multiple sensors
 * @res - resolution, will set to sensor->resolution
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

	if (!sensor)
		return struct_not_found_lib_ec;

	if (!obj)
		return dt_obj_not_found_lib_ec;

	if (obj->getDeviceCount() != devices_count)
		return device_not_found_lib_ec;

	TEMPS_SET_SENSOR_TO_ZERO(*sensor);

	obj->getAddress(sensor->address, index);
	obj->setResolution(sensor->address, res);
	sensor->obj = obj;
	sensor->resolution = res;

	return 0;
}


uint8_t _refresh_sensor(struct temp_sensor *sensor)
{
/* TODO: handle possible read errors */
	uint16_t time = 0;
	uint8_t ret = 0;

	switch (sensor->resolution) {		/* setup ds18b20 sensor measurement time */
	case simple:
		time = 100;		/* value rounded; to 9 bit */
		break;
	case standard:
		time = 190;		/* value rounded; to 10 bit */
		break;
	case advanced:
		time = 350;		/* default value; to 11 bit */
		break;
	case special:
		time = 750;		/* defaut value; to 12 bit */
		break;
	default:
		time = 750;
		break;
	}

	if (sensor->_read_timer && (millis() - sensor->_read_timer >= time)) {
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

	for (uint8_t i = 0; i < service->simple_sensors_count; i++) {
		sensor = &(service->simple_sensors[i]);
		ret = _refresh_sensor(sensor);
		if (ret)
			sensor->errors++;
	}

	for (uint8_t i = 0; i < service->spec_sensors_count; i++) {
		sensor = &(service->spec_sensors[i]);
		ret = _refresh_sensor(sensor);
		if (ret)
			sensor->errors++;
	}

	return ret;
}
