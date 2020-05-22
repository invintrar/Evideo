#This target will compile all the file
all:
	gcc -o output main.c nrf24l01.c -lwiringPi
	./output
rec:
	gcc record.c `pkg-config --cflags --libs gtk+-3.0` -o ejec 
	./ejec
