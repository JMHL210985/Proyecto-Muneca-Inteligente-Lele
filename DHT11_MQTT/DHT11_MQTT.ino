/*
 * Envio de datos de T/H a Node-RED
 * Por: Armando Cruz, Domingo Rosales, Juan Manuel Hernández Lara.
 * Fecha: julio de 2022
 * 
 * Estre programa envioa los valores de temperatura y humedad del sensor DHT11
 * por MQTT para que sean recuperados por Node-RED. 
 * 
 * Componente     PinESP32CAM     Estados lógicos
 * ledStatus------GPIO 33---------On=>LOW, Off=>HIGH
 * ledFlash-------GPIO 4----------On=>HIGH, Off=>LOW
 * 
//    ESP32CAM       DHT11
//    5V             Vcc
//    GND            GND
//    GPIO2         DATA
 * 
 */

//Bibliotecas
#include <WiFi.h>  // Biblioteca para el control de WiFi
#include <PubSubClient.h> //Biblioteca para conexion MQTT
#include "DHT.h" //Biblioteca del DHT11

//Constastes
#define DHTPIN 2 //Pin el que voy a conectar el sensor DHT11
#define DHTTYPE DHT11 //Variable para seleccion de modelo

// Datos de Red

// =====================================
// ID y contraseña de WiFi
// =====================================
const char* ssid = "Megacable_PTKuJdD";
const char* password = "EK4rpsypYQ9n43hMG2";


//Datos del broker MQTT
const char* mqtt_server = "18.195.236.18"; // Si estas en una red local, coloca la IP asignada, en caso contrario, coloca la IP publica
IPAddress server(18,195,236,18);

// Objetos
WiFiClient espClient; // Este objeto maneja los datos de conexion WiFi
PubSubClient client(espClient); // Este objeto maneja los datos de conexion al broker
DHT dht(DHTPIN, DHTTYPE); //Objeto para manejo del DHT11

// Variables
int flashLedPin = 4;  // Para indicar el estatus de conexión
int statusLedPin = 33; // Para ser controlado por MQTT
long timeNow, timeLast; // Variables de control de tiempo no bloqueante
int data = 0; // Contador
int wait = 5000;  // Indica la espera cada 5 segundos para envío de mensajes MQTT

// Inicialización del programa
void setup() {
  // Iniciar comunicación serial
  Serial.begin (115200);
  pinMode (flashLedPin, OUTPUT);
  pinMode (statusLedPin, OUTPUT);
  digitalWrite (flashLedPin, LOW);
  digitalWrite (statusLedPin, HIGH);

  Serial.println();
  Serial.println();
  Serial.print("Conectar a ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password); // Esta es la función que realiz la conexión a WiFi
 
  while (WiFi.status() != WL_CONNECTED) { // Este bucle espera a que se realice la conexión
    digitalWrite (statusLedPin, HIGH);
    delay(500); //dado que es de suma importancia esperar a la conexión, debe usarse espera bloqueante
    digitalWrite (statusLedPin, LOW);
    Serial.print(".");  // Indicador de progreso
    delay (5);
  }
  
  // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  // Si se logro la conexión, encender led
  if (WiFi.status () > 0){
  digitalWrite (statusLedPin, LOW);
  }
  
  delay (1000); // Esta espera es solo una formalidad antes de iniciar la comunicación con el broker

  // Conexión con el broker MQTT
  client.setServer(server, 1883); // Conectarse a la IP del broker en el puerto indicado
  client.setCallback(callback); // Activar función de CallBack, permite recibir mensajes MQTT y ejecutar funciones a partir de ellos
  delay(1500);  // Esta espera es preventiva, espera a la conexión para no perder información

  //Inicio del objeto que maneja el sensor
  dht.begin();
  
  timeLast = millis (); // Inicia el control de tiempo
}// fin del void setup ()

// Cuerpo del programa, bucle principal
void loop() {
  //Verificar siempre que haya conexión al broker
  if (!client.connected()) {
    reconnect();  // En caso de que no haya conexión, ejecutar la función de reconexión, definida despues del void setup ()
  }// fin del if (!client.connected())
  client.loop(); // Esta función es muy importante, ejecuta de manera no bloqueante las funciones necesarias para la comunicación con el broker
  
  timeNow = millis(); // Control de tiempo para esperas no bloqueantes
  if (timeNow - timeLast > wait) { // Manda un mensaje por MQTT cada cinco segundos
    timeLast = timeNow; // Actualización de seguimiento de tiempo

    //Obtener las lecturas de temperatura y humedad del sensor
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    //Comprobar que la conexion con el sensor sea exitosa
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    
    char dataString[8]; // Define una arreglo de caracteres para enviarlos por MQTT, especifica la longitud del mensaje en 8 caracteres
    dtostrf(t, 1, 2, dataString);  // Esta es una función nativa de leguaje AVR que convierte un arreglo de caracteres en una variable String
    Serial.print("Temperatura: "); // Se imprime en monitor solo para poder visualizar que el evento sucede
    Serial.println(dataString);
    client.publish("codigoIoT/LELE/temp_amb", dataString); // Esta es la función que envía los datos por MQTT, especifica el tema y el valor

    delay (100);
    dtostrf(h, 1, 2, dataString);
    Serial.print("Humedad: "); // Se imprime en monitor solo para poder visualizar que el evento sucede
    Serial.println(dataString);
    client.publish("codigoIoT/LELE/hum", dataString);
  }// fin del if (timeNow - timeLast > wait)
}// fin del void loop ()

// Funciones de usuario

// Esta función permite tomar acciones en caso de que se reciba un mensaje correspondiente a un tema al cual se hará una suscripción
void callback(char* topic, byte* message, unsigned int length) {

  // Indicar por serial que llegó un mensaje
  Serial.print("Llegó un mensaje en el tema: ");
  Serial.print(topic);

  // Concatenar los mensajes recibidos para conformarlos como una varialbe String
  String messageTemp; // Se declara la variable en la cual se generará el mensaje completo  
  for (int i = 0; i < length; i++) {  // Se imprime y concatena el mensaje
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  // Se comprueba que el mensaje se haya concatenado correctamente
  Serial.println();
  Serial.print ("Mensaje concatenado en una sola variable: ");
  Serial.println (messageTemp);

  // En esta parte puedes agregar las funciones que requieras para actuar segun lo necesites al recibir un mensaje MQTT

  
}// fin del void callback

// Función para reconectarse
void reconnect() {
  // Bucle hasta lograr conexión
  while (!client.connected()) { // Pregunta si hay conexión
    Serial.print("Tratando de contectarse...");
    // Intentar reconexión
    if (client.connect("ESP32CAMClient")) { //Pregunta por el resultado del intento de conexión
      Serial.println("Conectado");
    }// fin del  if (client.connect("ESP32CAMClient"))
    else {  //en caso de que la conexión no se logre
      Serial.print("Conexion fallida, Error rc=");
      Serial.print(client.state()); // Muestra el codigo de error
      Serial.println(" Volviendo a intentar en 5 segundos");
      // Espera de 5 segundos bloqueante
      delay(5000);
      Serial.println (client.connected ()); // Muestra estatus de conexión
    }// fin del else
  }// fin del bucle while (!client.connected())
}// fin de void reconnect(
