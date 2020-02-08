#!/usr/bin/python3

"""
	@Author = Darwinzh
	@Date = 11/21/2019

"""
import numpy as np
import cv2
import time

cap = cv2.VideoCapture(0)

# Obtenemos la resolucion por defecto
frame_width = int(cap.get(3))
frame_height = int(cap.get(4))

# Codificacion para la grabacion
codec = cv2.VideoWriter_fourcc('M', 'J', 'P', 'G')

# Definimos el codec y el nombre del archivo
out = cv2.VideoWriter( 'output' + '.avi', codec, 10, (frame_width, frame_height))

while(cap.isOpened()):
	ret, frame = cap.read()
	if ret == True:
		# Escribro las framas
		out.write(frame)

		cv2.imshow('Frame', frame)
		if cv2.waitKey(1) & 0xFF == ord('q'):
			break
	else:
		break
# Liberamos toda las cosas si el trabajo termina
cap.release()
out.release()
cv2.destroyAllWindows()
