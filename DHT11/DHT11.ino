/*
 * Control de encendido de camara,  Sentor infrarojo 
 * de temperatura (MLX90614) y sensor de temperatura 
 * y humedad ambiental(DHT11) con sensor Ultrasonico 
 * (HC-SR04)
 * 
 * Por: Juan Manuel Hernández Lara
 * Fecha: 18 de julio de 2022
 * 
 * Este programa controla el encendido de la camara del ESP32CAM  
 * al ser detectado un objeto con el sensor ultasonico (HC-SR04)
 * si es identifica con la camara una persona como usuario entonces
 * es activado el sensor infrarojo de temperatura (MLX90614) para medir su
 * temperatura, así mismo es activada la medicion de temperarutura 
 * y humedad ambiental mediante el sensor DHT11.
 * ESP32CAM      HC-SR04
        5V        Vcc
        GND       GND
        GPIO4     TRIG
        GPIO2     ECHO 

* ESP32CAM       MLX90614
    5V/3V3        VIN
    GND           GND
    GPIO15/1 (SCL)   SCL
    GPIO14/3 (SDA)   SDA

SCL: pin de señal de reloj de la interfaz I2C .
SDA: pin de señal de datos de la interfaz I2C.

* ESP32CAM       DHT11
    5V            Vcc
    GND           GND
    GPI13         DATA

 * Componente     PinESP32CAM     Estados lógicos
 * ledStatus------GPIO 33---------On=>LOW, Off=>HIGH
 * ledFlash-------GPIO 4----------On=>HIGH, Off=>LOW
 */

// Bibliotecas

#include <DHT.h>  // Biblioteca para manejar el sensor DHT-11

//Selecconamos pin de datos del sensor DHT
#define DHTPIN 13     

//Seleccionamos modelo de sensor de temperatura comentando y descomentando
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Datos de Red


// Objetos

// Variables del programa


// Inicialización del sensor DHT.

DHT dht(DHTPIN, DHTTYPE);

// Inicialización del programa
void setup() 
{
  //Configuración de pines
  
  //Inicialización de comunicación serial
  Serial.begin (115200);
  Serial.println(F("DHTxx test!"));
  dht.begin();
 
  // Iniciar el WiFi
  
}

  // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
  
  
//Cuerpo del programa, bucle principal
void loop() 
{
  delay(2000); //Es un sensor lento, por lo que hay que darle tiempo.
  float hambiente = dht.readHumidity(); //Lee la humedad del sensor y la asigna a hambiente
  float tambiente = dht.readTemperature();//Lee la temperatura del sensor y la asigna a tambiente
 

  if (isnan(hambiente) || isnan(tambiente)) //Verifique si alguna lectura falló
  {
    Serial.println(F("¡Falla al leer el sensor DHT!"));
    return;
  }
  Serial.print(F("Humedad: "));
  Serial.print(hambiente);
  Serial.print(F("% Temperatura: "));
  Serial.print(tambiente);
  Serial.println(F("°C "));
}
// Fin de void loop
