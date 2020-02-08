#include <gtk/gtk.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define LED 7

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    wiringPiSetup();
  	

    pinMode(LED,OUTPUT);

	
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "data.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    gtk_builder_connect_signals(builder, NULL);


    g_object_unref(builder);

    gtk_widget_show(window);                

    gtk_main();
    return 0;
}

// called when window is closed
void on_window_destroy()
{
    gtk_main_quit();
}

void on_led(GtkWidget *wid, gpointer ptr)
{
	digitalWrite(LED,HIGH);
	}

void off_led(GtkWidget *wid, gpointer ptr)
{
	digitalWrite(LED,LOW);
}

void video(GtkWidget *wid, gpointer ptr)
{
	system("sudo ffmpeg -f oss -f video4linux2 -i /dev/video0 /home/pi/out.mpg");
}

void videof(GtkWidget *wid, gpointer ptr)
{
	system("sudo killall ffmpeg");
}
