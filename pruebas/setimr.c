/*********** setitimer.c file *********/
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>

int count = 0;
struct itimerval t;
void timer_handler(int sig)
{
    printf("timer_handler: signal=%d count=%d\n", sig, ++count);
    if (count>=8)
    {
    printf("cancel timer\n");
    t.it_value.tv_sec = 0;
    t.it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &t, NULL);
    }
}
int main()
{
    struct itimerval timer;
    // Install timer_handler as SIGVTALRM signal handler
    signal(SIGVTALRM, timer_handler);
    // Configure the timer to expire after 100 msec
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 100000; // 100000 nsec
    // and every 1 sec afterward
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    // Start a VIRTUAL itimer
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
    printf("looping: enter Control-C to terminate\n");
    while(1);
}