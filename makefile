#This target will compile all the file
all:
	gcc -o output guiMain.c -lwiringPi `pkg-config --libs gtk+-3.0` `pkg-config --cflags gtk+-3.0`
	./output
	rm output
