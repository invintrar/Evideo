#!/usr/bin/python3

import serial

# Creamos el objeto serial
ser = serial.Serial('/dev/ttyS0')

try:
	while True:
		line = ser.readline()
		print(line)

except KeyboardInterrupt:
	ser.close()

