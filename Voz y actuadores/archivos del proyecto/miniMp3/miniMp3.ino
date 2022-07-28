/********************************************************************************
 * miniMp3.ino
 * control de salida de audio para indicaciones del sistema
 * @author  [Angelo qiao](Angelo.qiao@dfrobot.com)  2016/02/07 
 * Modificado por Domingo Rosales, Armando Cruz, Juan Manuel Hernandez version Julio 2022
 * este programa controla las salidas de audio para dar indicaciones de las 
 * actividades que se estan realizando con los actuadores
 * y el control por voz
 **/

 
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //use softserial para la comunicacion con  mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.porfavor revise la coneccion!"));
    Serial.println(F("2.por favor inserte SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(500); //Establezca el tiempo de espera de la comunicación en serie en 500 ms
  //----Set volume----
  myDFPlayer.volume(10);  //valor de volumne (0~30).
  myDFPlayer.volumeUp(); //volumen alto
  myDFPlayer.volumeDown(); //Volumen bajo
  //----sistema de equalizacion----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
//  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  //----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);
  //----Mp3 control----
//  myDFPlayer.sleep();     //sleep
//  myDFPlayer.reset();     //Reset the module
//  myDFPlayer.enableDAC();  //Enable On-chip DAC
//  myDFPlayer.disableDAC();  //Disable On-chip DAC
//  myDFPlayer.outputSetting(true, 15); //configuración de salida, habilite la salida y establezca la ganancia en 15
  //----Mp3 play----
  myDFPlayer.next();  //Play next mp3
  delay(1000);
  myDFPlayer.previous();  //Play previo mp3
  delay(1000);
  myDFPlayer.play(1);  //Play primer mp3
  delay(1000);
  myDFPlayer.loop(1);  //Loop the ultimo mp3
  delay(1000);
  myDFPlayer.pause();  //pausa  mp3
  delay(1000);
  myDFPlayer.start();  //inicia el mp3 despues de l apausa
  delay(1000);
  myDFPlayer.playFolder(15, 4);  //reproducir mp3 específico en SD:/15/004.mp3; Nombre de carpeta (1~99); Nombre de archivo (1~255)
  delay(1000);
  myDFPlayer.enableLoopAll(); //loop  mp3 .
  delay(1000);
  myDFPlayer.disableLoopAll(); //para el loop .
  delay(1000);
  myDFPlayer.playMp3Folder(4); //reproducir mp3 específico en SD:/MP3/0004.mp3; Nombre de archivo (0~65535)
  delay(1000);
  myDFPlayer.advertise(3); //anunciar mp3 específico en SD:/ADVERT/0003.mp3; Nombre de archivo (0~65535)
  delay(1000);
  myDFPlayer.stopAdvertise(); //dejar de anunciar
  delay(1000);
  myDFPlayer.playLargeFolder(2, 999); //play especificoc mp3 en SD:/02/004.mp3; nombre de carpeta(1~10); nombre archivo(1~1000)
  delay(1000);
  myDFPlayer.loopFolder(5); //loop todos los archivos mp3 dentro del folder SD:/05.
  delay(1000);
  myDFPlayer.randomAll(); //Random lista de  mp3.
  delay(1000);
  myDFPlayer.enableLoop(); //avilita loop.
  delay(1000);
  myDFPlayer.disableLoop(); //disable loop.
  delay(1000);
  //---leyendo informacion 
  Serial.println(myDFPlayer.readState()); //leer estado de mp3
  Serial.println(myDFPlayer.readVolume()); //leer volumne volume
  Serial.println(myDFPlayer.readEQ()); //leer sistema de equializacion
  Serial.println(myDFPlayer.readFileCounts()); //leer los archivos de la SD card
  Serial.println(myDFPlayer.readCurrentFileNumber()); //leer la lista de los nombres de los archivos
  Serial.println(myDFPlayer.readFileCountsInFolder(3)); //leer la continuacion de las carpetas dentro de la SD:/03
}
void loop()
{
  static unsigned long timer = millis();
  if (millis() - timer > 3000) {
    timer = millis();
    myDFPlayer.next();  //ejecuta 3 segundos el  mp3
  }
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Imprima el mensaje detallado de DFPlayer para manejar diferentes errores y estados.
  }
}
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("fuera de tiempo!"));
      break;
    case WrongStack:
      Serial.println(F("pila incorrecta!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Inserte targeta!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Targeta removida!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("targeta en linea!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Numero:"));
      Serial.print(value);
      Serial.println(F(" play terminado!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Targeta no funciona"));
          break;
        case Sleeping:
          Serial.println(F("Reproducciendo"));
          break;
        case SerialWrongStack:
          Serial.println(F("Obtener pila incorrecta"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Compruebe que la suma no coincide"));
          break;
        case FileIndexOut:
          Serial.println(F("Índice de archivo fuera de límite"));
          break;
        case FileMismatch:
          Serial.println(F("No se puede encontrar el archivo"));
          break;
        case Advertise:
          Serial.println(F("en anunciar"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
