#ifndef TEMPS_LIB_H
#define TEMPS_LIB_H

#include <stdint.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*	Overview:
 *
 *	Library for management temperature sensors, in control about every sensor:
 *	struct temp_sensor:
 *		*obj		ptr to DallasTemperature class that control this sensor
 *		address		address of this sensor
 *		resolution	special (12 bit) or simple (9 bit)
 *		cur_temp	current temp, updating in background temps_lib_refresh() func
 *		prev_temp	previous temp, updating with every chage cur_temp:
 *					so, cur_temp will be as prev_temp, new val -> in cur_temp
 *		tar_temp	just value to comparing
 *		changes_timer	millis(), when was entered into .prev_temp;
 *		errors		if some errors in the read/cmp/other proccess, it's will be > 0
 *		_read_timer	internal timer to between request temps
 *
 *	struct temps_service:
 *		simple_sensors		array of simple sensors
 *		simple_sensors_count	length of simple_sensors array
 *		spec_sensors		array of special sensors
 *		spec_sensors_count	length of spec_sensors array
 *		
 *
 *	You must define:
 *		#define TEMPS_USE_DS18B20
 *
 *
 *	Usage:
 *
 *	#define CNT_SPEC_SENSORS 2
 *	#define CNT_SIMPLE_SENSORS 3
 *
 *	// here register DallasTemperature objects
 *	TEMPS_REGISTER_SENSORS_PIN(warn_sensor, 8);
 *	TEMPS_REGISTER_SENSORS_PIN(top_sensor, 9);
 *	TEMPS_REGISTER_SENSORS_PIN(def_sensor, 10);
 *
 *	// just arrays, without set OneWire/DallasTemperature
 *	TEMPS_REGISTER_ARR(simple_sns_arr, CNT_SIMPLE_SENSORS);
 *	TEMPS_REGISTER_ARR(special_sns_arr, CNT_SPEC_SENSORS);
 *
 *	void some_init_func(struct temps_service *temps)
 *	{
 *		temps->simple_sensors = simple_sns_arr;
 *		temps->simple_sensors_count = CNT_SIMPLE_SENSORS;
 *		temps->spec_sensors = special_sns_arr;
 *		temps->spec_sensors_count = CNT_SPEC_SENSORS;
 *
 *		// sensor found or not, for example, next do it for all your sensors
 *		if (warn_sensor.getDeviceCount() != 1)
 *			return ERROR
 *		...	// other sensors
 *
 *		// next get/set addresses, DallasTemperature obj:
 *		warn_sensor.getAddress(temps->spec_sensor[0].address, 0);
 *				// where [0] - index of this spec sensor in arr, your value
 *		temps->spec_sensors[0].obj = &warn_sensor;	// [0] - ditto
 *		temps->spec_sensors[0].resolution = special;	// [0] - ditto
 *
 *		TODO: doc for simple sensors setup
 *	}
 *
 *
 *	NOTE:
 *		begin() func of DallasTemperature obj IS NOT called ANYWHERE here.
 *		setWaitForConversion() func of DallasTemperature obj IS NOT called ANYWHERE here.
 *		You need to call it yourself for your objects
 *
 *		TODO: add general init func to do it
 */

#ifndef TEMPS_USE_DS18B20
#define TEMPS_USE_DS18B20
#endif

/*
 * Register new OneWire, DallasTemperature objects to management sensors
 * Call once for each temperature pin
 *
 * @name - name for new DallasTemperature object
 * @pin - pin on which the sensor/sensors are located
 */
#define TEMPS_REGISTER_SENSORS_PIN(name, pin)			\
	static OneWire name ##_wire((pin));			\
	static DallasTemperature name(&(name ## _wire))

/*
 * Register new OneWire, DallasTemperature objects to managment MULTIPLE SENSORS (<127)
 *
 * Where 'name' - name for DallasTemperature object
 */
#define TEMPS_REGISTER_SIMPLE_SENSORS(name, pin)		\
	static OneWire name ## _wire((pin));			\
	static DallasTemperature name(&(name ## _wire));	\

/*
 * Register new array of 'struct temp_sensors'
 *
 * Where 'name' - name of array, 'count' - count of items in array
 */
#define TEMPS_REGISTER_ARR(name, count)				\
	static struct temp_sensor (name)[(count)]

/*
 * Set all fields of sensor to 0/NULL
 */
#define TEMPS_SET_SENSOR_TO_ZERO(sensor)			\
	do {							\
		(sensor).obj = NULL;				\
		/*(sensor).address = 0;		*/		\
		(sensor).resolution = simple;  /* def simple */	\
		(sensor).cur_temp = 0;				\
		(sensor).prev_temp = 0;				\
		(sensor).tar_temp = 0;				\
		(sensor).changes_timer = 0;			\
		(sensor).errors = 0;				\
		(sensor)._read_timer = 0;			\
	} while(0)


/* alias to float, 255 mean 25.5, 777 mean 77.7, 1115 mean 111.5, etc, one sign afer dot */
typedef uint16_t fl_t;


#ifdef TEMPS_USE_DS18B20
enum accuracy {
			/* resolution	time */
	simple = 9,	/* 0.5 C	93.75 ms*/
	standard = 10,	/* 0.25 C	187.5 ms*/
	advanced = 11,	/* 0.125 C	350 ms */
	special = 12	/* 0.0625 C	750 ms */
};

enum {
	device_not_found_lib_ec = 60,
	struct_not_found_lib_ec = 61,
	dt_obj_not_found_lib_ec = 62,	/* DallasTemperature */
};
#endif


struct temp_sensor {
	DallasTemperature *obj;
	DeviceAddress address;

	enum accuracy resolution;
	fl_t cur_temp;
	fl_t prev_temp;
	fl_t tar_temp;
	uint32_t changes_timer;		/* millis(), when value was entered into 'prev_temp' */
	uint8_t errors;
	uint32_t _read_timer;
};

struct temps_service {
	struct temp_sensor *simple_sensors;	/* arr */
	uint8_t simple_sensors_count;

	struct temp_sensor *spec_sensors;	/* arr */
	uint8_t spec_sensors_count;
};


uint8_t temps_lib_init_sensor(struct temp_sensor *sensor,
			      DallasTemperature *obj,
			      enum accuracy res,
			      uint8_t index,
			      uint8_t devices_count);
uint8_t temps_lib_refresh(struct temps_service *service);


#endif
