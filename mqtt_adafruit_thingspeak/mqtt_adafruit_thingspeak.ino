
#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DHT.h>
#define DHTPIN 2 //pino digital para o sensor de umidade e temperatura
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "GVT-080B"
#define WLAN_PASS       "32216512"
int status = WL_IDLE_STATUS;

/************************* MQTT Setup *********************************/

#define SERVER      "mqtt.thingspeak.com"
#define SERVERPORT  1883                   // use 8883 for SSL
#define USERNAME    "priscyllat"
#define MQTT_KEY    "RTLB4NDHEYFD43CU"

/************ Global State (you don't need to change this!) ******************/
#include<SoftwareSerial.h>
SoftwareSerial Serial1(3, 4);
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiEspClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT, USERNAME, MQTT_KEY);

/****************************** Feeds ***************************************/
unsigned long myChannelNumber = 452132;
const char * myWriteAPIKey = "FTXKGNZ7M6UIT8V9";
// Notice MQTT paths follow the form: channels/<channelId>/publish/fields/field<fieldNumer>
Adafruit_MQTT_Publish temperatura = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field1/FTXKGNZ7M6UIT8V9");
Adafruit_MQTT_Publish umidade = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field2/FTXKGNZ7M6UIT8V9");
Adafruit_MQTT_Publish chuva = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field3/FTXKGNZ7M6UIT8V9");
int chuvaDigital = 6; //Pino ligado ao D0 do sensor
int chuvaAnalogico = A0; //Pino ligado ao A0 do sensor
long chuvaDigitalValor = 0;
long chuvaAnalogicoValor = 0;
/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {
  pinMode(chuvaDigital, INPUT);
  pinMode(chuvaAnalogico, INPUT);
  Serial.begin(9600);
  delay(10);
  dht.begin();
  Serial.println(F("MQTT - Estacao Climatica IoT"));
  Serial1.begin(19200);

  WiFi.init(&Serial1);
  //Connect to WiFi access point.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  //attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WLAN_SSID);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WLAN_SSID, WLAN_PASS);
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  
    MQTT_connect();
    
    float t = dht.readTemperature(); // Read temperature from DHT sensor.
    float h = dht.readHumidity();  // Read humidity from DHT sensor
   
    // Now we can publish stuff!
    Serial.print(F("\nEnviando temperatura "));
    Serial.print(t);
    Serial.print("...");
    if (! temperatura.publish(t)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }
    
    delay15s();
    
    Serial.print(F("\nEnviando umidade "));
    Serial.print(h);
  
    if (! umidade.publish(h)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }

    //Le e arnazena o valor do pino digital
    chuvaDigitalValor  = digitalRead(chuvaDigital);
    //Le e armazena o valor do pino analogico
    chuvaAnalogicoValor = analogRead(chuvaAnalogico);
    //Envia as informacoes para o serial monitor
    Serial.print("Valor digital : ");
    Serial.print(chuvaDigitalValor);
    Serial.print(" - Valor analogico : ");
    Serial.println(chuvaAnalogicoValor);

    delay15s();
    // Now we can publish stuff!
    Serial.print(F("\nEnviando temperatura "));
    Serial.print(chuvaDigitalValor);
    Serial.print("...");
    if (! chuva.publish(chuvaDigitalValor)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }
 
    delay30m();
 
}

void delay30m(){
  for(int i = 0; i < 1800; i++){
    delay(1000);
  }
}
void delay15s(){
  for(int i = 0; i < 15; i++){
    delay(1000);
  }
}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
