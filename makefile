#This target will compile all the file
all:
	gcc -o output main.c nrf24l01.c -lwiringPi
	./output
