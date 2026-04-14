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
 *	RW	*sensor_data	sensor-specific data; need for read_temp() and request_temp()
 *
 *	R	cur_temp	current temp, updating in background temps_lib_refresh() func
 *	R	prev_temp	previous temp, updating with every chage cur_temp:
 *					so, cur_temp will be as prev_temp, new val -> in cur_temp
 *	RW	tar_temp	just value to comparing
 *	R	changes_timer	millis(), when was entered into .prev_temp;
 *	R	errors		if some errors in the read/cmp/other proccess, it's will be > 0
 *	RW	is_enable	if 0, sensor is not serviced, other fields are not updated
 *	RW	req_interval	request temp from sensor no more often than every req_interval ms
 *
 *	 W	read_temp	sensor-specific temperature request function;
 *					the library thinks that you set this function;
 *					the library calls it automatcally every 'req_interval' ms
 *	 W	request_temp	sensor-specific temperature update function;
 *					the library thinks that you set this function;
 *					the library calls it automatcally
 *
 *		// internal data, you don't need to change it:
 *		_read_timer	timer between temp requests
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


/* alias to float, 255 mean 25.5, 777 mean 77.7, 1115 mean 111.5, etc, one sign afer dot */
typedef uint16_t fl_t;


#ifdef TEMPS_USE_DS18B20

#define DS18B20_MAX_TEMP 125
#define DS18B20_MIN_TEMP -55

#define DS18B20_9_BIT_TIME 95
#define DS18B20_10_BIT_TIME 190
#define DS18B20_11_BIT_TIME 350
#define DS18B20_12_BIT_TIME 750

struct ds18b20 {
	DallasTemperature *obj;
	DeviceAddress address;
};

fl_t read_ds18b20(struct temp_sensor *sensor);
void request_ds18b20(struct temp_sensor *senfor);

#endif /* TEMPS_USE_DS18B20 */


#define TEMPS_SENSOR_DATA_INVALID 22
#define TEMPS_READ_TEMP_INVALID 23
#define TEMPS_REQUEST_TEMP_INVALID 24


struct temp_sensor {
	/**
	 * sensor-specific data;
	 * store here everything you need for read_temp() and request_temp() funtions
	 */
	void *sensor_data;

	fl_t cur_temp;
	fl_t prev_temp;
	fl_t tar_temp;
	uint32_t changes_timer;		/* millis(), when value was entered into 'prev_temp' */
	uint32_t data;			/* user data about this sensor */
	uint32_t req_interval;		/* request temp no more often than every req_interval ms */
	uint8_t errors;
	bool is_enable;
	uint32_t _read_timer;

	fl_t (*read_temp)(struct temp_sensor *sensor); /* func to read finished temp */
	void (*request_temp)(struct temp_sensor *sensor); /* func to request new temp */
};

struct temps_service {
	struct temp_sensor *sensors;		/* arr */
	uint8_t sensors_count;
};


uint8_t temps_lib_init(struct temp_sensor *sensor);
uint8_t temps_lib_refresh(struct temps_service *service);
uint8_t *temps_lib_convert(fl_t num, uint8_t buff[5], uint8_t is_float);


#endif
