/**
 * @author:DzhL
 * @date : 2/8/2020
 */

#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <gtk/gtk.h>

static uint8_t run;
uint8_t segundos, minutos, horas;
char tmp[1024];

void intHandler();
void acumulador();

void main(int argc, char *argv[])
{
    segundos = minutos = horas = 0;

    sprintf(tmp, "ffmpeg -i /dev/video0 -t 00:00:%d -r 24 -metadata title='Estacion Video' video.avi",30);

    //Catch Ctrl-C
    signal(SIGINT,intHandler);
    
    g_timeout_add_seconds(1,(GSourceFunc) acumulador, NULL);
    
    // Process video frames
    gtk_main();

    return 0;
}

void acumulador()
{
    if(minutos == 1 && segundos == 10)
    {
        system(tmp);
        
    }
    if(segundos <= 58)
    {
        segundos++;
    }
    else
    {
        segundos = 0;
        if(minutos <= 58)
        {
            minutos++;
        }
        else
        {
            horas++;
            minutos = 0;
        }
    }

    printf("Hora: %02d:%02d:%02d\n ",horas,minutos,segundos);
}

//Catch Ctrl C
void intHandler(int dummy)
{
    gtk_main_quit();
}

/**
 * End File
 */
