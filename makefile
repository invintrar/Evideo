#This target will compile all the file
all:
	gcc -o out main.c nrf24l01.c -lwiringPi
