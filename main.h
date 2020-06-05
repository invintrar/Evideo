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
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>
#include "nrf24l01.h"


/**
 *          Define macros
 */

#define TO_NSEC(t)          (((long)t[0] * 1000000000L) + t[1])
#define _XOPEN_SOURCE       700
#define TIMES               10
#define SECS_IN_DAY         (24 * 60 * 60)
#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define LED 		  	    7
#define ledSetOutput()      pinMode(LED, OUTPUT)
#define ledOn()     	    digitalWrite(LED, HIGH)
#define ledOff() 	        digitalWrite(LED, LOW)


/**
 *          Define variables
 */
struct timespec ts;//sttruct get time
struct itimerval timer; // configure timer
//Init flag for nrf start in mode transmition
uint8_t bNrf = 0;
// This variable use for first data receive.
uint8_t bInit =1;
//Use for ending bucle while 1
uint8_t run = 1;
//Address Nrf24L01+
uint8_t tx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t rx_addr[5] = {0x78, 0x78, 0x78, 0x78, 0x78};
//Data sent or receive Nrf24L01+
uint8_t txEnv[SIZEDATA] = {0};
uint8_t rxRec[SIZEDATA] = {0};
//Variable get time 
uint8_t tarea = 0;
int timeNanoSeconds = 0;
int timeSeconds = 0;
// variable temporal for formate output string
char tmp[1024];
// Use for execute proceso in terminal
int ret;
// Use count 
unsigned int cSync = 0;
bool bSync = true;
// Time sent get from mastater
int t1[2] = {0};
int t3[2] = {0};
// Prompter for crear file.txt 
FILE *archivo;


long ms_diff = 0;
long sm_diff = 0;
long sum_offset = 0;
long sum_delay = 0;

/**
 *          Function prototype
 */
void interrupcionNRF(void);
void interruptTimer(int sig);
void interruptProcessEnd(int sig);
void intHandler(int dummy);
void task(uint8_t opc);
void syncClock(void);
void getTime(int in[2]);
void setClock(clockid_t clock, struct timespec *ts);
void videoCapture(void);
void displayClock(clockid_t clock, char *name);
void ledToggle(void);
uint8_t existFile(void);
void convertCharToInt(int out[2]);
long syncDiffMS(int t1[2]);
long delayDifSM(int t4[2]) ;


#endif

/**
 * End File
 */