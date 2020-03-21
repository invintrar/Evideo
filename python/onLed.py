#!/usr/bin/python3

'''
@author: DzhL
'''

import RPi.GPIO as gpio
from time import sleep

gpio.setwarnings(False)
gpio.setmode(gpio.BCM)
gpio.setup(4, gpio.OUT, initial = gpio.LOW)

try:
	while True:
		gpio.output(4,gpio.HIGH)
		sleep(.250)
		gpio.output(4,gpio.LOW)
		sleep(.250)
except KeyboardInterrupt:
	gpio.output(4, gpio.LOW)

