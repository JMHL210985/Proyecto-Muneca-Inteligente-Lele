"""********************************************************************************
  sonidoMp3.ino
  control de salida de audio para indicaciones del sistema
  @author  [Angelo qiao](Angelo.qiao@dfrobot.com)  2016/02/07 
  Modificado por Domingo Rosales, Armando Cruz, Juan Manuel Hernandez version Julio 2022
  este programa controla las salidas de audio para dar indicaciones de las 
  actividades que se estan realizando con los actuadores
  y el control por voz
 
"""
import subprocess
subprocess.call("mplayer fichero.mp3".split())
