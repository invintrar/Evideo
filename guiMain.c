/*
@author: darwinZh
*/

#include <gtk/gtk.h>
#include <wiringPi.h>
#include <stdio.h>

#define LED 7

void end_program(GtkWidget *wid, gpointer ptr)
{
	gtk_main_quit();
}

void on_led(GtkWidget *wid, gpointer ptr)
{
	char buffer[30];
	digitalWrite(LED,HIGH);
	sprintf(buffer, "Led Encendido");
	gtk_label_set_text(GTK_LABEL(ptr), buffer);
}

void off_led(GtkWidget *wid, gpointer ptr)
{
	char buffer[30];
	digitalWrite(LED,LOW);
	sprintf(buffer, "Led Apagado");
	gtk_label_set_text(GTK_LABEL(ptr),buffer);
}

int main (int argc, char *argv[])
{
	wiringPiSetup();
	pinMode(LED,OUTPUT);
	gtk_init (&argc, &argv);

	GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget *btn = gtk_button_new_with_label("Close window");
	g_signal_connect (btn, "clicked", G_CALLBACK (end_program), NULL);
	g_signal_connect (win, "delete_event", G_CALLBACK (end_program), NULL);

	GtkWidget *lbl = gtk_label_new(" ");
	GtkWidget *lbl2 = gtk_label_new(" ");

	GtkWidget *enc = gtk_button_new_with_label("Encender Led");
	g_signal_connect (enc, "clicked", G_CALLBACK (on_led), lbl);

	GtkWidget *apg = gtk_button_new_with_label("Apagar Led");
	g_signal_connect (apg, "clicked", G_CALLBACK (off_led), lbl2);


	GtkWidget *box = gtk_box_new (FALSE, 5);
	gtk_box_pack_start(GTK_BOX (box), enc, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (box), lbl, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX (box), apg, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (box), lbl2, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (box), btn, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(win), box);
	gtk_widget_show_all(win);
	gtk_main();
	return 0;
}
