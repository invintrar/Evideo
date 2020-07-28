#for use glib.h add `pkg-config --cflags --libs glib-2.0`
all:
	gcc main.c nrf24l01.c -lwiringPi -o out
exec:
	sudo ./out > /dev/null &
