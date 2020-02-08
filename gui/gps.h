#ifndef GPS_H
#define GPS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <math.h>

typedef struct{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t gradosLatitud;
	uint8_t minutosLatitud;
	uint8_t segundosLatitud;
	uint8_t latitud;
	uint8_t gradosLongitud;
	uint8_t minutosLongitud;
	uint8_t segundosLongitud;
	uint8_t longitud;
	uint8_t day;
	uint8_t month;
	uint8_t year;
}dataGps;

//Variables externas
extern int serialPort;

uint8_t initSerial(void);
dataGps getDataGps(void);

#endif
/* End File */


