"""
  reconocimientodevozlele.py
  Sistema de reconocimiento de voz con raspberry 
  author ejemplo de la librera SpeechRecognition 
  Modificado  por  Domingo Rosales, Armando Cruz, Juan Manuel Hernandez version Julio 2022
  este programa es una prueba el reconocimiento de voz y traduccion a texto de la orden indicadancendido y apagado de un led desde la Raspberry Pi
  para su futuro uso con el control de actuadores
  
"""
# importar la libreria de reconocimeinto de voz SpeechRecognition
import speech_recognition as sr
import subprocess   #importar libreria para reproduccion de audio
import RPi.GPIO as GPIO #libreria para controlar los puertos de la libreria
GPIO.setwarnings(False) #On désactive les messages de alerta

rel1 = 10 # asigan numero de GPIO para los relays
rel2 = 11
rel3 = 13
GPIO.setmode(GPIO.BOARD) #inicializa los puerto GPIO
GPIO.setup(rel1, GPIO.OUT) #inicializa los puertos como salida
GPIO.setup(rel2, GPIO.OUT)
GPIO.setup(rel3, GPIO.OUT)

 
r = sr.Recognizer()   #crea la instancia r
op =0
while op != 10:
    
    with sr.Microphone() as source:  #ajuata el microfono y elimina ruidos y se pone en epera de escuchae la voz
        print('Esperando que ablen : ') 
        audio = r.listen(source) # Guarda el audio depues de eqie se realiz una pausa en el abla
        
        # Captura de errores  en caso de existir alguno 
        try:
            # se pasa el audio a texto para analizar  las cadenas de texto
            text = r.recognize_google(audio, language ='es-MX') # el indicativo es-MX es para el lenguaje en español de mexico
            test=text.split()
            print('usted dijo: ',test)
            
            # revisando que tenga ma s de una palabra        
            if len(test) <= 2: #revisamos la cadena por si es en adios
                palabras=['adiós', 'terminar','salir']
                if any(palabra in test for palabra in palabras):#si se encuentra alguna coincidencia se manda el mesaje de voz
                    subprocess.call("mplayer 12.mp3".split())  # mensaje de lele agradesiendo el uso del sitema
                    GPIO.output(rel1, GPIO.LOW) # apaga los relays
                    GPIO.output(rel2, GPIO.LOW)
                    GPIO.output(rel3, GPIO.LOW)
                palabras=['lele', 'hola']
                if any(palabra in test for palabra in palabras):
                    print("aqui entro")
                    #si se encuentra alguna coincidencia se manda el mesaje de voz( "hola lele" o "buen dia lele")
                    subprocess.call("mplayer 1.mp3".split())  # mensaje de lele saludando
            else:# el mensaje ablado cuenta con mas de dos palabras se deve analizar las palabras claves para ver la instrucion
                #Revisamos la coincidencia para indicar la instruccion Abrir la puerta
                palabras=['abre','puerta']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( puerta abierta")
                    subprocess.call("mplayer 2.mp3".split())  # mensaje de lele
                    GPIO.output(rel1, GPIO.HIGH) #activa el relevador de apertura de puerta
                    
                    #Revisamos la coincidencia para indicar la instruccion Cerrar la puerta
                palabras=['lele','Cierra','puerta']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( puerta cerrada")
                    subprocess.call("mplayer 3.mp3".split())  # mensaje de lele
                    GPIO.output(rel1, GPIO.LOW) #activa el relevador cerrado de puerta
                            
                #Revisamos la coincidencia para indicar la instruccion prender television
                palabras=['prende','televisión']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( prender television)
                    subprocess.call("mplayer 4.mp3".split())  # mensaje de lele
                    GPIO.output(rel2, GPIO.HIGH) #activa el relevador de prender televisor
                            
                #Revisamos la coincidencia para indicar la instruccion Apagar televisión
                palabras=['apaga','televisión']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( apagar television)
                    subprocess.call("mplayer 5.mp3".split())  # mensaje de lele
                    GPIO.output(rel2, GPIO.LOW) #activa el relevador apaga televisor
                                            
                #Revisamos la coincidencia para indicar la instruccion prender la luz
                palabras=['prende','luz']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( prender la luz)
                    subprocess.call("mplayer 6.mp3".split())  # mensaje de lele
                    GPIO.output(rel3, GPIO.HIGH) #activa el relevador prende luz
                            
                            
                #Revisamos la coincidencia para indicar la instruccion Apagar la luz
                palabras=['lele','apaga','luz']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( luz apagada)
                    subprocess.call("mplayer 7.mp3".split())  # mensaje de lele
                    GPIO.output(rel3, GPIO.LOW) #activa el relevador Apaga luz
                            
                #Revisamos la coincidencia para indicar como tomar temperatura
                palabras=['lele','tomar','temperatura']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( de como tomar la temperatura)
                    subprocess.call("mplayer 8.mp3".split())  # mensaje de lele 
        
                #Revisamos la coincidencia para indicar como detectar una persona
                palabras=['detectar','persona']
                if all(palabra in test for palabra in palabras):
                    #si se encuentran todas las coincidencias a se manda el mesaje de voz( de como detectar una persona)
                    subprocess.call("mplayer 10.mp3".split())  # mensaje de lele 
                        
        except:   # en caso de no camturar sonido se lanza la exepcion
            print('lo siento no entendi') 
