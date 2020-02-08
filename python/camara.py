#!/usr/bin/python3
"""
	@Author:DzhL
	@Date : 11/22/2019
"""

import numpy as np
import cv2

cap = cv2.VideoCapture(0)

while(True):
	# Leemos un frame y lo guardamos
	valido, img = cap.read()

	# Si el frame se ha capturado correctamente, continuamos
	if valido:

		# Convertimos la imagen a blanco y negro
		#img_gris = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

		# Mostramos la imagen
		cv2.imshow('Imagen', img)

		# Con la tecla 'q' salimos del programa
		if cv2.waitKey(1) & 0xFF == ord('q'):
			break

cap.release()
cv2.destroyAllWindows()
