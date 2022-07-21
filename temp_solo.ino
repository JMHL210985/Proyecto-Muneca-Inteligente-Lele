#include <DHT_U.h>
#include <DHT.h>

//Codigo de temperatura. Modificado de:
// Example testing sketch for various DHT humidity/temperature sensors written by ladyada//

// Objetos
// Variables
float t = 0; // Temperatura en °C
float f = 0; // Temperatur en °F
float h = 0; // Humedad

#define DHTPIN 12   

#define DHTTYPE DHT11   

 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
    
  
   dht.begin(); // Inicia dht
   delay(1000);  // Esta espera es preventiva, espera a la conexión para no perder información

   
}// fin del void setup ()
    



// Cuerpo del programa, bucle principal
void loop() {
  delay(5000);
//leer temperatura y humedad
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  h = dht.readHumidity();
  
  if ( isnan(t) || isnan(f)) {
    Serial.println(F("No hay conexion"));
    return;
  }
   
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("\t");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("\n");
  
    
}// fin del void loop ()
