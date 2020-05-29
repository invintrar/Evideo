#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define _XOPEN_SOURCE 700
#define SECS_IN_DAY (24 * 60 * 60)

struct timespec ts;
struct itimerval timer; // configure timer
unsigned char cont = 1;

static void displayClock(clockid_t clock, char *name)
{    
    if (clock_gettime(clock, &ts) == -1) 
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    printf("%-15s: %10ld.%03ld (", name,
    (long) ts.tv_sec, ts.tv_nsec / 1000000);
    long days = ts.tv_sec / SECS_IN_DAY;
    if (days > 0)
        printf("%ld days + ", days);
    printf("%2ldh %2ldm %2lds", (ts.tv_sec % SECS_IN_DAY) / 3600,
    (ts.tv_sec % 3600) / 60, ts.tv_sec % 60);
    printf(")\n");
}


void timer_handler(int sig)
{
    
    if(cont <= 1)
    {
        printf("Timer_handler: signal=%d\n", sig);
        displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
        cont++;
        
    }else
    { 
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);    
    }
        
}


static void setClock(clockid_t clock, time_t tSec, long tnSec)
{
    struct timespec tp = {tSec, tnSec};
    if(clock_settime(clock, &tp) == -1 )
    {
        perror("clock_settime");
        exit(EXIT_FAILURE);
    }
}


int main(void)
{
    /*
    timeSet.tm_year = 2020 - 1900;
    timeSet.tm_mon =  3;
    timeSet.tm_mday = 27;
    timeSet.tm_hour = 17;
    timeSet.tm_min = 48;
    timeSet.tm_sec = 14;
    timeSet.tm_isdst = 0;
    t = mktime(&timeSet);
    if (t == -1)
        perror("unknown");
    else {
        printf("seconds since the Epoch: %ld\n", (long) t); 
        setClock(CLOCK_REALTIME,t, 0);
    }*/
    // Install timer_handler as SIGVTALRM signal handler
    signal(SIGALRM, timer_handler);
    displayClock(CLOCK_REALTIME, "CLOCK_REALTIME");
    timer.it_value.tv_sec = 5;
    timer.it_value.tv_usec = 0; // start in 5 sec
    timer.it_interval.tv_sec = 10;
    timer.it_interval.tv_usec = 0; // period = 60 sec
    setitimer(ITIMER_REAL, &timer, NULL);

    while(1);

    return 0;
}
