/**
 * @author:DzhL
 * @date : 2/8/2020
 */

#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <unistd.h>



static uint8_t run;
uint8_t segundos, minutos, horas;
char tmp[1024];
char buffer[12];

void intHandler();
void acumulador();

uint8_t be(void)
{
    FILE *arch;
    arch = fopen("video.avi","r");
    if (arch == NULL)
    {
        return 0;
    }
    else
    {
        fclose(arch);
        return 1;  
    }
}

int main(int argc, char *argv[])
{
    segundos = minutos = horas = 0;

    if(be())
    {
        sprintf(tmp, "rm video.avi");
        system(tmp);
        printf("Archivo video.avi borrado");
    }else{
        printf("No hay archivo para borrar");
    }
    //Catch Ctrl-C
    signal(SIGINT,intHandler);
    
    g_timeout_add_seconds(1,(GSourceFunc) acumulador, NULL);
    
    // Process video frames
    gtk_main();

    return 0;
}

void acumulador()
{
    if(minutos == 0 && segundos == 30)
    {
        printf("\nInicio de Grabacion: %02d min %02d s" , minutos, segundos); 
        sprintf(tmp, "ffmpeg -i /dev/video0 -t 00:00:%d -r 24 -metadata title='Estacion Video' -loglevel quiet video.avi &",10);
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

    printf("\nHora: %02d:%02d:%02d",horas,minutos,segundos);
}

//Catch Ctrl C
void intHandler(int dummy)
{
    gtk_main_quit();
}

/**
 * End File
 */
