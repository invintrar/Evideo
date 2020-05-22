/**
 * @author: DzhL
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include "nrf24l01.h"


uint8_t bNrf, bTog;
// This variable use for first data receive.
uint8_t bInit;

//Address Nrf24L01+
uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};

//Data sent or receive Nrf24L01+
uint8_t txEnv[12] = {0};
uint8_t rxRec[12] = {0};

//Variable get time
struct tm *ptr;
time_t t;
uint8_t tarea = 0;
uint16_t time_us = 0;

//Use for ending bucle infinite
static uint8_t run;

//Function prototype
void interrupcion(void);
void intHandler(int dummy);
void tareas(uint8_t tarea);


char tmp[1024];

#define LED 		  	7

#define Led_SetOutput() pinMode(LED, OUTPUT)
#define Led_SetHigh() 	digitalWrite(LED, HIGH)
#define Led_SetLow() 	digitalWrite(LED, LOW)

#endif

/**
 * End File
 */
