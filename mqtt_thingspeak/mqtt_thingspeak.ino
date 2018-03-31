

 /*
  estacao-metereologica-iot

  Autora: Priscylla Tavares
  Descricao: Projeto que pretende fazer a leitura de sensor de umidade,
  temperatura e chuva, e envia esses dados para serviço através do
  protocolo MQTT. 
  Utiliza a placa Arduino Uno, sensor DHT11 e sensor de chuva.
*/
#include <SPI.h>
#include <DHT.h>
#include<SoftwareSerial.h>
#include <PubSubClient.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>



#define DHTPIN 2 //pino digital para o sensor de umidade e temperatura
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
#define LIGHTPIN A0 // Analog light sensor connected to analog pin A0.


char ssid[] = "GVT-080B";            // your network SSID (name)
char pass[] = "0000000";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char mqttUserName[] = "priscyllat";  // Can be any name.
char mqttPass[] = "RTLB4NDHEYFD43CU";  // Change this your MQTT API Key from Account > MyProfile.
char writeAPIKey[] = "FTXKGNZ7M6UIT8V9";    // Change to your channel Write API Key.
long channelID = 452132;

                              
#include<SoftwareSerial.h>
SoftwareSerial Serial1(3, 4);
//SoftwareSerial esp8266(3, 4);
WiFiEspClient client;  // Initialize the Wifi client library.
PubSubClient mqttClient(client); // Initialize the PuBSubClient library.

const char* server = "mqtt.thingspeak.com"; 

unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 20L * 1000L; // Post data every 20 seconds.
void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("Sataus: DHT11 funcionando");
  Serial1.begin(19200);
  Serial.println("Inicializando WiFi...");

 // initialize ESP module
  WiFi.init(&Serial1);
// check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

    // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  //testeComunicacaoEsp8266();
  //conectarWIFI();
   printWifiStatus();

  mqttClient.setServer(server, 1883);   // Set the MQTT broker details.

}

void loop() {
//  // Tempo de leitura de temperatura e umidade: 250 millisegundos
//  // A leitura do sensor também pode ter 2 segundos de atraso para atualização
//  float h = dht.readHumidity();
//  // Leitura da temperatura em Celsius
//  float t = dht.readTemperature();
//
//  // Checa se a leitura falhou e retorna para tentar novamente
//  if (isnan(h) || isnan(t)) {
//    Serial.println("Failed to read from DHT sensor!");
//    return;
//  }
//
//  Serial.print("Umidade: ");
//  Serial.print(h);
//  Serial.println(" %\t");
//  Serial.print("Temperatura: ");
//  Serial.print(t);
//  Serial.println(" *C ");
//  
//  delay(3000);


  // Reconnect if MQTT client is not connected.
  if (!mqttClient.connected()) 
  {
    reconnect();
  }

  mqttClient.loop();   // Call the loop continuously to establish connection to the server.

  // If interval time has passed since the last connection, publish data to ThingSpeak.
  if (millis() - lastConnectionTime > postingInterval) 
  {
    mqttpublish();
  }
}

//void testeComunicacaoEsp8266(){
//  //testando comunicação com o módulo WiFi
//  esp8266.flush();
//  esp8266.println("AT");
//  delay(2000);
//    
//  if(esp8266.find("OK")){
//    Serial.println("Comunicacao com modulo ESP8266: OK");
//  }
//  else {
//    Serial.println("Erro no modulo ESP8266");
//  }
//}

void mqttpublish() {
  
  float t = dht.readTemperature(); // Read temperature from DHT sensor.
  float h = dht.readHumidity();  // Read humidity from DHT sensor.
  int lightLevel = analogRead(LIGHTPIN); // Read from light sensor
  
  // Create data string to send to ThingSpeak
  String data = String("field1=" + String(t, DEC) + "&field2=" + String(h, DEC) + "&field3=" + String(lightLevel, DEC) + "&status=MQTTPUBLISH" );
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer,length+1);
  Serial.println(msgBuffer);
  
  // Create a topic string and publish data to ThingSpeak channel feed. 
  String topicString ="channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);
  length=topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer,length+1);
 
  mqttClient.publish( topicBuffer, msgBuffer );

  Serial.println(topicBuffer);
  Serial.println(msgBuffer);
  
  lastConnectionTime = millis();
}

void reconnect() 
{
  char clientID[10];

  // Loop until reconnected.
  while (!mqttClient.connected()) 
  {
    Serial.println("Attempting MQTT connection...");
//    // Generate ClientID
//    for (int i = 0; i < 8; i++) {
//        clientID[i] = alphanum[random(51)];
//    }

    // Connect to the MQTT broker
    if (mqttClient.connect("56b7758bb37b46528dec1d0184e53060",mqttUserName,mqttPass)) 
    {
      Serial.print("Connected with Client ID:  ");
      Serial.print(String("56b7758bb37b46528dec1d0184e53060"));
      Serial.print(", Username: ");
      Serial.print(mqttUserName);
      Serial.print(" , Passwword: ");
      Serial.println(mqttPass);
    } else 
    {
      Serial.print("failed, rc=");
      // Print to know why the connection failed.
      // See http://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

//boolean conectarWIFI(){
//  Serial.println("Conectando a WiFi...");
//  String cmd ="AT+CWMODE=1";
//  esp8266.println(cmd);
//  delay(2000);
//  esp8266.flush(); //clear buffer
//  cmd="AT+CWJAP=\"";
//  cmd+=SSID;
//  cmd+="\",\"";
//  cmd+=PASSWORD;
//  cmd+="\"";
//  esp8266.println(cmd);
//  delay(5000);
//  
//  if(esp8266.find("OK")){
//    Serial.println("Conectado com sucesso!");
//    return true;
//  }else{
//    Serial.println("Falha na conexao!");
//    return false;
//  }
//  Serial.println();
//}


