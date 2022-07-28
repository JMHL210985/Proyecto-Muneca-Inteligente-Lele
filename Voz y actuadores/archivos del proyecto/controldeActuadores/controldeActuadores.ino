/********************************************************************************
 * controldeActuadores.ino
 * control de actuadres por ordenes a voz
 * @author  JiapengLi 2013/06/13    vercion inicial
 * Modificado por Domingo Rosales, Armando Cruz, Juan Manuel Hernandez version Julio 2022
 * este programa controla treas actuadores por medios de comados de voz reconicodos por el modulo 
 * Voice Reconogtion modeule v3 y por medio de actuadores se activan los siguintes objetos:
 * Activacion o pagado de luces, Encendido de Televisor  y apertura o cerrado de puerta
 * asi como la informacion de bienvenida al uso de este dispositivo 
 **/
  
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

/**        
  Connecion  
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
   5v  ------->     Vcc
   Gnd ------->     Gnd
   Arduino   DFPlayer Mini
   10  ------->     TX
   11  ------->     RX
   5V  ------->     Vcc
   GND ------->     GND
   Arduino     Reley
   8   ------->     In1
   7   ------->     In2
   6   ------->     In3
*/

VR myVR(2,3);    // 2:RX 3:TX,
uint8_t records[7]; // Gearda comandos de voz
uint8_t buf[64];

int led = 13;
int rele1 = 8;
int rele2 = 7;
int rele3 = 6;

#define onRecord    (0)
#define offRecord   (1) 

**
  @brief Imprimir firma, si el carácter es invisible,
           imprimir valor hexadecimal en su lugar.
  @param buf --> longitud del comando
           len --> número de parámetros
*/

void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

/**
  @brief Imprimir firma, si el carácter es invisible,
           imprimir valor hexadecimal en su lugar.
  @param buf --> Valor de retorno del módulo VR cuando se reconoce la voz.
             buf[0] --> Modo de grupo (FF: Ninguno Grupo, 0x8n: Usuario, 0x0n: Sistema
             buf[1] --> número de registro que se reconoce.
             buf[2] --> Valor del índice (posición) del reconocedor del registro reconocido.
             buf[3] --> Longitud de la firma
             buf[4]~buf[n] --> Firma
*/

void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGrupo\tNumeroRegistro\tAsignacion");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup()
{
  /** initialize */
  myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Control de actuadores");
  
  pinMode(led, OUTPUT);
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
    
  if(myVR.clear() == 0){
    Serial.println("Reconocedor borrado.");
  }else{
    Serial.println("no se encontro modulo.");
    Serial.println("Verifique la conexión y reinicie Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)onRecord) >= 0){
    Serial.println("Record Leido");
  }
  
  if(myVR.load((uint8_t)offRecord) >= 0){
    Serial.println("Registro cargado");
  }
}

void loop()
{
  /* REvisamos la oracion recibida por el sistema y planteamos la actividad correapondiente */
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case saludo:
        /** Saludos inicial  */
        Serial.println("saludos inicial");
        // voz indicando saludos del sistema 
        break;
      case puertaA:
        /** orden a actuador abrir puerta*/
        Serial.println("Abrir puerta");
        digitalWrite(rele8, HIGH)
        /** voz indicando que puerta se abrio **/
        break;
      case puertaC:
        /** orden a actuador Cerrar puerta*/
        Serial.println("Cerrar puerta");
        digitalWrite(rele8,LOW)
        /* voz indicando que puerta se abrio */
        break;
      case teleP:
        /** orden a actuador Prender Television*/
        Serial.println("prender Television");
        digitalWrite(rele7, HIGH)
        /*voz indicando que puerta se abrio */
        break;
      case teleA:
        /** orden a actuador Apagar Television*/
        Serial.println("Apagar Television");
        digitalWrite(rele7,LOW)
        /** voz indicando Apagar Television */
        break;
      case luzP:
        /** orden a actuador Prender luz*/
        Serial.println("prender luz");
        digitalWrite(rele6, HIGH)
        /*voz indicando que se prendio la luz */
        break;
      case luzA:
        /** orden a actuador Apagar luz*/
        Serial.println("Apagar Luz");
        digitalWrite(rele6,LOW)
        /** voz indicando que se apogo la luz */
        break;
        
      default:
        Serial.println("Funcion indefinida");
        // voz indicando que no se encontro la orden dicatada
        
        break;
    }
    /** voice recognized */
    printVR(buf);
  }
}
