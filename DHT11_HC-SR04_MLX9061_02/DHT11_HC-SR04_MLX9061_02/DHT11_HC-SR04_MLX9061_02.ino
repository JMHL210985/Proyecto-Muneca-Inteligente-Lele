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
        GPIO13    TRIG
        GPIO2     ECHO 

* ESP32CAM       MLX90614
    5V/3V3        VIN
    GND           GND
    GPIO15 (SCL)  SCL
    GPIO14 (SDA)  SDA

SCL: pin de señal de reloj de la interfaz I2C .
SDA: pin de señal de datos de la interfaz I2C.

* ESP32CAM       DHT11
    5V            Vcc
    GND           GND
    GPIO12        DATA

 * Componente     PinESP32CAM     Estados lógicos
 * ledStatus------GPIO 33---------On=>LOW, Off=>HIGH
 * ledFlash-------GPIO 4----------On=>HIGH, Off=>LOW
 */

// Bibliotecas

#include <DHT_U.h>
#include <DHT.h>// Biblioteca para manejar el sensor DHT-11
//#include <Adafruit_MLX90614.h>
#include <Wire.h>

 

 
//Selecconamos pin de datos del sensor DHT
#define DHTPIN 12   
//Seleccionamos modelo de sensor de temperatura comentando y descomentando
#define DHTTYPE DHT11  
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)



// Declaración de variables para pines de sensor HC-SR04
// Aquí indicamos a donde se conectan las señales del sensor

const int pinecho = 2;
const int pintrigger = 13;

// Datos de Red


// Objetos
//Se declara un objeto para el sensor de Temp. infrarojo
//Adafruit_MLX90614 mlx = Adafruit_MLX90614(); 
// Variables del programa
unsigned long tiempo, distancia;
int identificado = 1;
float t = 0; // Temperatura en °C
float f = 0; // Temperatur en °F
float h = 0; // Humedad

  

// Inicialización del sensor DHT.
   DHT dht(DHTPIN, DHTTYPE);
// Inicialización del programa
void setup() 
{


  //Configuración de pines
  
  //Inicialización de comunicación serial
  Serial.begin (115200);
//  Serial.println("Iniciando test del MLX90614");  
//  mlx.begin(); //Se inicia el sensor
  Serial.println(F("DHTxx test!"));
  dht.begin(); // Comienza el sensor DHT.
  delay(1000);  // Esta espera es preventiva, espera a la conexión para no perder información

 
 // configurar los pines utilizados por el sensor HC-SR04
  pinMode(pinecho, INPUT);
  pinMode(pintrigger, OUTPUT);
 
  // Iniciar el WiFi
  
}

  // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
  
  
//Cuerpo del programa, bucle principal
void loop() 
{
  delay(5000); //El sensor DHT es lento, por lo que hay que darle tiempo.
  t = dht.readTemperature();///Lee la temperatura en grados centigrados del sensor y la asigna a t
  f = dht.readTemperature(true);//Lee la temperatura en grados Fahreheit del sensor y la asigna a f
  h = dht.readHumidity();//Lee la humedad relativa del sensor y la asigna a h

  if ( isnan(t) || isnan(f)) //Verifique si alguna lectura falló
  {
    Serial.println(F("No hay conexion"));
    return;
  }
 
 
  // asegurarnos que el pin trigger se encuentra en estado bajo
  digitalWrite(pintrigger, LOW);
  delayMicroseconds(2);
 
  // comenzamos pulso alto, debe durar 10 uS
  // luego regresamos a estado bajo
  digitalWrite(pintrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pintrigger, LOW);
 
  // medimos el tiempo en estado alto del pin "echo"
  // el tiempo en estado alto es proporcional a la distancia medida
  tiempo = pulseIn(pinecho, HIGH);
 
  // LA VELOCIDAD DEL SONIDO ES DE 340 M/S O 29 MICROSEGUNDOS POR CENTIMETRO
  // DIVIDIMOS EL TIEMPO DEL PULSO ENTRE 58, TIEMPO QUE TARDA RECORRER IDA Y
  // VUELTA UN CENTIMETRO LA ONDA SONORA
  distancia = tiempo / 58;
 
  // imprimir la distancia medida al monitor serial
  Serial.print(F("Distancia: "));
  Serial.print(distancia);
  Serial.println(F(" cm"));
  if (distancia < 50 && identificado!= 0)
    {
      Serial.print("Humedad relativa: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperatura: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print("\t");
      Serial.print(f);
      Serial.print(" *F\t");
      Serial.print("\n");
  
      
      //Se imprimen los datos de sensor
//      Serial.print("Temperatura Ambiente= "); 
//      Serial.print(mlx.readAmbientTempC()); Serial.println(" °C");
//      Serial.print("Temperatura del Objeto= "); 
//      Serial.print(mlx.readObjectTempC()); Serial.println(" °C"); 
      //Serial.print("Temperatura Ambiente = "); 
      //Serial.print(mlx.readAmbientTempF()); Serial.println(" °F"); 
      //Serial.print("Temperatura del Objeto = ");  
      //Serial.print(mlx.readObjectTempF()); Serial.println(" °F"); 
      
      delay(500);
    }
 
}
// Fin de void loop
