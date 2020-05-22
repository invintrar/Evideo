/**
 * @author: DzhL
 * @date  : 5/22/2020
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <gtk/gtk.h>


#define led     7
#define ledSetOutput()      pinMode(led, OUTPUT);
#define ledOn()             digitalWrite(led, HIGH)
#define ledOff()            digitalWrite(led, LOW) 


// Variable for time
typedef struct
{
    uint8_t  seconds;
    uint8_t  minutes;
    uint8_t  hours;
    uint16_t useconds;
}timeSync;

char tmp[1024];

/**
 * function prototype
 */

void interruptSeconds();
void catchCtrlC(int dummy);


#endif

