/**
 * @uthor: DzhL
 * @date : 5/22/2020
 */

#include "main.h"

/**
 * function main
 */
int main( int argc, char *argv[])
{
//    timeSync timp1;

    wiringPiSetup();

    ledSetOutput();
    
    //catch ctrl C
    signal(SIGINT, catchCtrlC);

    g_timeout_add_seconds(1,(GSourceFunc) interruptSeconds, NULL);

    // bucle infinite
    gtk_main();

} // end main

// Interrupt each seconds
void interruptSeconds()
{
    static gboolean running = TRUE; 
    if (running)
    {
        ledOn();
    }
    else
    {
        ledOff();
    } // end if else

    running = !running;

} // end interrupt Seconds


// Catch ctrl-C
void catchCtrlC(int dummy)
{
    gtk_main_quit();
    ledOff();
} // end catch ctrlC

/**
 * end file
 */
