"""
  pruebavoz.py
  Sistema de reconocimiento de voz con raspberry 
  author ejemplo de la librera SpeechRecognition 
  Modificado  por  Domingo Rosales, Armando Cruz, Juan Manuel Hernandez version Julio 2022
  este programa es una prueba el reconocimiento de voz y traduccion a texto de la orden 
  indicada aparecera en texto con lo cual haremos la siguiente parte del sistema
  para su futuro uso con el control de actuadores
  
"""


import speech_recognition as sr
r = sr.Recognizer()   #crea la instancia r
 
with sr.Microphone() as source:  #ajuata el microfono y elimina ruidos y se pone en epera de escuchae la voz
    print('Esperando que ablen : ') 
    audio = r.listen(source) # Guarda el audio depues de eqie se realiz una pausa en el abla
    
    # Captura de errores  en caso de existir alguno 
    try:
		# se pasa el audio a texto para analizar  las cadenas de texto
		text = r.recognize_google(audio, language ='es-MX') # el indicativo es-MX es para el lenguaje en español de mexico
		test=text.split()
		print('usted dijo: ',test)
	except:   # en caso de no camturar sonido se lanza la exepcion
		print('lo siento no entendi') 
