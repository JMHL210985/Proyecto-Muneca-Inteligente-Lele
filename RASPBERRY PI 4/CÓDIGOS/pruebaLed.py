""""
pruebaLed.py
  control de prendido y apagado de led desde Raspberry pi 
   Modificado  por  Domingo Rosales, Armando Cruz, Juan Manuel Hernandez version Julio 2022
  este programa es una prueba de encendido y apagado de un led desde la Raspberry Pi
  para su futuro uso con el control de actuadores
"""


import RPi.GPIO as GPIO
import time
ESPERA = 0.5
rel1 = 10
rel2 = 11
rel3 = 13
GPIO.setmode(GPIO.BOARD)
GPIO.setup(rel1, GPIO.OUT)
GPIO.setup(rel2, GPIO.OUT)
GPIO.setup(rel3, GPIO.OUT)
while True:
    GPIO.output(rel1, GPIO.HIGH)
    GPIO.output(rel2, GPIO.HIGH)
    GPIO.output(rel3, GPIO.HIGH)
    time.sleep(ESPERA)
    GPIO.output(rel1, GPIO.LOW)
    GPIO.output(rel2, GPIO.LOW)
    GPIO.output(rel3, GPIO.LOW)
    time.sleep(ESPERA)
