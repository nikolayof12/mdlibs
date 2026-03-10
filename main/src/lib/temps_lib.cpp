#include <stdint.h>

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "lib/temps_lib.h"


uint8_t temps_lib_init(struct temps_service *service)
{
/* TODO: here setup the addresses of every sensor, etc... */
}


uint8_t temps_lib_refresh(struct temps_service *service)
{
/* TODO: here read the temps, update the timers, etc... */
}
