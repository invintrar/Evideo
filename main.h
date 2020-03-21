/**
 * @author: DzhL
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include "nrf24l01.h"
#include <time.h>

#define LED 		  	7

#define Led_SetOutput() pinMode(LED, OUTPUT)
#define Led_SetHigh() 	digitalWrite(LED, HIGH)
#define Led_SetLow() 	digitalWrite(LED, LOW)

#endif

/**
 * End File
 */
