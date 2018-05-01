
#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DHT.h>
#define DHTPIN 2 //pino digital para o sensor de umidade e temperatura
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

/********************************** Configuração WiFi ****************************************/

#define WLAN_SSID       "GVT-080B"
#define WLAN_PASS       "32216512"
int status = WL_IDLE_STATUS;

/********************************** Configuração MQTT ***************************************/

#define SERVER      "mqtt.thingspeak.com"
#define SERVERPORT  1883                   // use 8883 for SSL
#define USERNAME    "priscyllat"
#define MQTT_KEY    "RTLB4NDHEYFD43CU"

/***************************** Estado Global ************************************************/
#include<SoftwareSerial.h>
SoftwareSerial Serial1(3, 4);
// Configura o cliente MQTT passando o cliente WiFi e o login do servidor MQTT.
WiFiEspClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT, USERNAME, MQTT_KEY);

/****************************** Configuração dos campos ***************************************/
unsigned long myChannelNumber = 452132;
const char * myWriteAPIKey = "FTXKGNZ7M6UIT8V9";
// Notice MQTT paths follow the form: channels/<channelId>/publish/fields/field<fieldNumer>
Adafruit_MQTT_Publish temperatura = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field1/FTXKGNZ7M6UIT8V9");
Adafruit_MQTT_Publish umidade = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field2/FTXKGNZ7M6UIT8V9");
Adafruit_MQTT_Publish chuva = Adafruit_MQTT_Publish(&mqtt, "channels/452132/publish/fields/field3/FTXKGNZ7M6UIT8V9");

/*********************************** Variáveis ***********************************************/
int chuvaDigital = 6; //Pino ligado ao D0 do sensor
int chuvaAnalogico = A0; //Pino ligado ao A0 do sensor
long chuvaDigitalValor = 0;
long chuvaAnalogicoValor = 0;

/************************************* Sketch ************************************************/

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
  //Conecta a WiFi.
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(WLAN_SSID);

  //tentativa de conectar a rede WiFi
  while ( status != WL_CONNECTED) {
    Serial.print("Tentando conectar a WPA de SSID: ");
    Serial.println(WLAN_SSID);
    //Conecta a rede WPA/WPA2 network
    status = WiFi.begin(WLAN_SSID, WLAN_PASS);
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());


}

void loop() {
//Se certifica de que há conecxão ao servidor MQTT está viva
//Isso fará a primeira conexão e automaticamente reconecta quando desconectado.  
  
    MQTT_connect();
    
    float t = dht.readTemperature(); // Lê a temperatura do sensor DHT.
    float h = dht.readHumidity();  // Lê a umidade do sensor DHT.
   
    // Plublicando temperatura
    Serial.print(F("\nEnviando temperatura "));
    Serial.print(t);
    Serial.print("...");
    if (! temperatura.publish(t)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }

    //delay necessário para o thinkspeak
    delay15s();

    // Plublicando umidade
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
    
   //Plublicando se houve chuva
    Serial.print(F("\nEnviando se ocorreu chuva"));
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
// Função que conecta e reconecta quando necessário ao servidor MQTT.
// Deve ser chamada no loop e manipulará a conexão.
void MQTT_connect() {
  int8_t ret;

  // Para se já conectado.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Conectando ao MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect retornará 0 para conectado
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Tentando novamente a conexão MQTT em 5 segundos...");
       mqtt.disconnect();
       delay(5000);   // espera 5 segundos
       retries--;
       if (retries == 0) {
          // Espera pelo WTD para se resetar
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
