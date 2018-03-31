/*
 WiFiEsp example: WebClient

 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/
#include <DHT.h>
#include <ThingSpeak.h>
#include "WiFiEsp.h"
unsigned long myChannelNumber = 452132;
const char * myWriteAPIKey = "FTXKGNZ7M6UIT8V9";


#define DHTPIN 2 //pino digital para o sensor de umidade e temperatura
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
// Emulate Serial1 on pins 6/7 if not present
//#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(3, 4); // RX, TX
//#endif

char ssid[] = "GVT-080B";            // your network SSID (name)
char pass[] = "00000000";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

//char server[] = "thingspeak.com"; 

// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(19200);
  // initialize ESP module
  dht.begin();
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
  
  printWifiStatus();
  ThingSpeak.begin(client);
 
}

void loop()
{
  
 float t = dht.readTemperature(); // Read temperature from DHT sensor.
 float h = dht.readHumidity();  // Read humidity from DHT sensor
 ThingSpeak.setField(1, t);
 ThingSpeak.setField(2, h);
 ThingSpeak.setField(3, 1);
 ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  //if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }

delay(30000);
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
