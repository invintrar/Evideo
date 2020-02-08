#!/usr/bin/python3

import time
import serial
port = "/dev/serial0"

def parseGPS(data):
	"""print "raw:", data prints raw data"""
	data = data.decode()
	if data[0:6] == "$GPRMC":
		sdata = data.split(",")
		if sdata[2] == 'V':
			print("No satellite data available")
			return
		print("----------Parsing GPGGA----------")
		ti =str((int(sdata[1][0:2]) - 5)) + ":" + sdata[1][2:4] + ":" + sdata[1][4:6]
		# latitude
		lat = decode(sdata[3])
		# latitude direction N/S
		dirLat = sdata[4]
		# longitute
		lon = decode(sdata[5])
		# longitute direction E/W
		dirLon = sdata[6]
		# Speed in knots
		speed = sdata[7]
		# True course
		trCourse = sdata[8]
		# Date
		da = sdata[9][0:2] + "/" + sdata[9][2:4] + "/" + sdata[9][4:6]

		print(" Date: %s   time: %s\n Latitude: %s %s\n Longitud: %s %s\n Speed: %s\n True Course: %s" %(da, ti, lat, dirLat, lon, dirLon, speed, trCourse))


def decode(coord):
	""" Converts DDMM.MMMMM > DD def MM.MMMMM min"""
	x = coord.split(".")
	head = x[0]
	tail = x[1]
	deg = head[0:-2]
	min = head[-2:]
	segu = (int(tail)*6)/1000

	return deg + "°" + min + "'" + "%.2f" % segu  + u'\u201d'


print ("Receiving GPS data")

ser = serial.Serial(port, baudrate = 9600, timeout = 1)

try:
	while True:
		data = ser.readline()
		parseGPS(data)

except KeyboardInterrupt:
	ser.close()
	print("Bye")
