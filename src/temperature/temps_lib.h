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
 *		is_enable	if 0, sensor is not serviced, other fields are not updated
 *
 *	General structure:
 *	struct temps_service:
 *		sensors		array of sensors
 *		sensors_count	length of sensors array
 *		
 *
 *	You must define:
 *		#define TEMPS_USE_DS18B20	// to use DS18B20 sensor
 *
 *
 *	Usage:
 *
 *	So, we have 2 sensors on D8 pin, and 3 on D10 pin;
 *	Sensors from D8 pin need set to 12 bit,
 *	Sensors from D10 pin - to 9 bit.
 *
 *	#define COUNT_OF_SENSORS 5
 *
 *	// here register DallasTemperature objects
 *	TEMPS_REGISTER_SENSORS_PIN(warn_sensors, 8);	// here 2 sensors
 *	TEMPS_REGISTER_SENSORS_PIN(def_sensors, 10);	// here 3
 *
 *	// just array, without set OneWire/DallasTemperature
 *	TEMPS_REGISTER_ARR(sensors_arr, ALL_COUNT_OF_SENSORS);
 *
 *	void some_init_func(struct temps_service *temps)
 *	{
 *		temps->sensors = sensors_arr;
 *		temps->sensors_count = ALL_COUNT_OF_SENSORS;
 *
 *		// sensor found or not, for example, next do it for all your sensors
 *		if (warn_sensor.getDeviceCount() != 2)
 *			return ERROR
 *		...	// other sensors
 *
 *		// need call before all temps_lib_init_sensor() sensors
 *		warn_sensors.begin();
 *		def_sensors.begin();
 *
 *		temps_lib_init_sensor(&temps->sensors[0],
 *				      &warn_sensors,
 *				      special,			// set resolution here
 *				      0, 1);
 *		temps_lib_init_sensor(&temps->sensors[1],
 *				      &warn_sensors,
 *				      special,
 *				      0, 1);
 *
 *
 *		temps_lib_init_sensor(&temps->sensors[2],
 *				      &def_sensors,
 *				      simple,
 *				      0, 0);
 *		temps_lib_init_sensor(&temps->sensors[3],
 *				      &def_sensors,
 *				      simple,
 *				      0, 1);
 *		temps_lib_init_sensor(&temps->sensors[4],
 *				      &def_sensors,
 *				      simple,
 *				      0, 2);
 *
 *		// manually set the async mode
 *		warn_sensors.setWaitForConversion(false);
 *		def_sensors.setWaitForConversion(false);
 *	}
 *
 *	Next you can computing cur_temp, tar_temp, prev_temp, changes_timer in bg function,
 *	in interrupts, in callback... as you want, here without this
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
 * Register new array of 'struct temp_sensors'
 * Call once for all of your sensors
 *
 * @name - name for new array
 * @count - count of items in array
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
		(sensor).data = 0;				\
		(sensor).errors = 0;				\
		(sensor)._read_timer = 0;			\
		(sensor).is_enable = 1;	/* default enable */	\
	} while (0)


/* alias to float, 255 mean 25.5, 777 mean 77.7, 1115 mean 111.5, etc, one sign afer dot */
typedef uint16_t fl_t;


#ifdef TEMPS_USE_DS18B20

#define DS18B20_MAX_TEMP 125
#define DS18B20_MIN_TEMP -55

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
#endif /* TEMPS_USE_DS18B20 */


struct temp_sensor {
	DallasTemperature *obj;
	DeviceAddress address;

	enum accuracy resolution;
	fl_t cur_temp;
	fl_t prev_temp;
	fl_t tar_temp;
	uint32_t changes_timer;		/* millis(), when value was entered into 'prev_temp' */
	uint32_t data;			/* user data about this sensor */
	uint8_t errors;
	bool is_enable;
	uint32_t _read_timer;
};

struct temps_service {
	struct temp_sensor *sensors;		/* arr */
	uint8_t sensors_count;
};


uint8_t temps_lib_init_sensor(struct temp_sensor *sensor,
			      DallasTemperature *obj,
			      enum accuracy res,
			      uint8_t index,
			      uint8_t devices_count);
uint8_t temps_lib_refresh(struct temps_service *service);
uint8_t *temps_lib_convert(fl_t num, uint8_t buff[5], uint8_t is_float);


#endif
