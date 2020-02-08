#!/usr/bin/python3
import spidev
import time


def ByteToHex(Bytes):
	return ''.join(["0x%02X " %  x for x in Bytes]).strip()
#end def

# Create spi object
spi  = spidev.SpiDev()
# open spi port 0, device (CS) 0
spi.open(0,0)
spi.bits_per_word = 8
spi.max_speed_hz = 10000000
spi.mode = 0
'''
print(spi.bits_per_word)
print(spi.cshigh)
print(spi.loop)
print(spi.lsbfirst)
print(spi.max_speed_hz)
print(spi.mode)
print(spi.threewire)
'''
a = 0x00

try:
	while True:
		#transfer one byte an receive one byte
		resp = spi.xfer2([a, 0x00])
		resp1 = ByteToHex([resp[1]])
		print("%4s %4s" %(hex(a), resp1))
		# sleep for 0.1 seconds
		time.sleep(0.1)
		if (a > 0x1C):
			spi.close()
			break
		#end if
		a = a + 1
	# end while
except KeyboardInterrupt:
	spi.close()
# end try
