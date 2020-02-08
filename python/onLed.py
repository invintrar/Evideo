#!/usr/bin/python3

'''
@author: DarwinZh
'''

import RPi.GPIO as gpio
from time import sleep

gpio.setwarnings(False)
gpio.setmode(gpio.BCM)
gpio.setup(4, gpio.OUT, initial = gpio.LOW)

try:
	while True:
		gpio.output(4,gpio.HIGH)
		sleep(1/2)
		gpio.output(4,gpio.LOW)
		sleep(1/2)
except KeyboardInterrupt:
	gpio.output(4, gpio.LOW)

