
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
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT, USERNAME, MQTT_KEY);

/****************************** Feeds ***************************************/
unsigned long myChannelNumber = 452132;
const char * myWriteAPIKey = "FTXKGNZ7M6UIT8V9";
// Notice MQTT paths follow the form: channels/<channelId>/publish/fields/field<fieldNumer>
Adafruit_MQTT_Publish temperatura = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field1/FTXKGNZ7M6UIT8V9");
Adafruit_MQTT_Publish umidade = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field2/FTXKGNZ7M6UIT8V9");

/*************************** Variaveis ************************************/
int timesCount = 0;

/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {
  Serial.begin(9600);
  delay(10);
dht.begin();
  Serial.println(F("Adafruit MQTT demo"));
  Serial1.begin(19200);

  WiFi.init(&Serial1);
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

// attempt to connect to WiFi network
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

  timesCount = 1800;

}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.

  if(timesCount == 1800){
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
  
    Serial.print(F("\nEnviando umidade "));
    Serial.print(h);
  
     if (! umidade.publish(h)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }
    timesCount = 0;
  }

  delay(1000);
  timesCount++;
  
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
