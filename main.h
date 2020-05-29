/**
 * @author: DzhL
 */

#ifndef MAIN_H
#define MAIN_H

/**
 * Define libraries
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>
#include "nrf24l01.h"


/**
 *          Define macros
 */

#define _XOPEN_SOURCE 700
#define SECS_IN_DAY (24 * 60 * 60)
#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define LED 		  	7
#define ledSetOutput()  pinMode(LED, OUTPUT)
#define ledOn()     	digitalWrite(LED, HIGH)
#define ledOff() 	    digitalWrite(LED, LOW)


/**
 *          Define variables
 */
struct timespec ts;
struct itimerval timer; // configure timer
//Init flag for nrf start in mode transmition
uint8_t bNrf = 4;
// This variable use for first data receive.
uint8_t bInit =1;
//Use for ending bucle while 1
uint8_t run = 1;
//Use for ending bucle while 2
uint8_t run1 = 1;
//Address Nrf24L01+
uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
//Data sent or receive Nrf24L01+
uint8_t txEnv[12] = {0};
uint8_t rxRec[12] = {0};
//Variable get time 
uint8_t tarea = 0;
int timeNanoSeconds = 0;
int timeSeconds = 0;
// variable temporal for formate output string
char tmp[1024];

/**
 *          Function prototype
 */
void interrupcion(void);
void task(uint8_t opc);
void intHandler(int dummy);
void syncClock(int times);
void getTime(void);
void setClock(clockid_t clock, struct timespec *ts);
void timer_handler(int sig);
void videoCapture(void);
void displayClock(clockid_t clock, char *name);
void ledToggle(void);

#endif

/**
 * End File
 */