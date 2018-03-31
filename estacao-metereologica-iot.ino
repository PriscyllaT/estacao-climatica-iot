
 /*
  estacao-metereologica-iot

  Autora: Priscylla Tavares
  Descricao: Projeto que pretende fazer a leitura de sensor de umidade,
  temperatura e chuva, e envia esses dados para serviço através do
  protocolo MQTT. 
  Utiliza a placa Arduino Uno, sensor DHT11 e sensor de chuva.
*/

#include <DHT.h>
#include<SoftwareSerial.h>
#include <PubSubClient.h>
//#include<ESP8266WiFi.h>

#define DHTPIN 2 //pino digital para o sensor de umidade e temperatura
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

#define SSID "GVT-080B";
#define PASSWORD "00000000"

SoftwareSerial monitor(3, 4);


void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("Sataus: DHT11 funcionando");
  monitor.begin(19200);
  Serial.println("Inicializando WiFi...");
 
  testeComunicacaoEsp8266();
  conectarWIFI();

}

void loop() {
  // Tempo de leitura de temperatura e umidade: 250 millisegundos
  // A leitura do sensor também pode ter 2 segundos de atraso para atualização
  float h = dht.readHumidity();
  // Leitura da temperatura em Celsius
  float t = dht.readTemperature();

  // Checa se a leitura falhou e retorna para tentar novamente
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");
  
  delay(3000);
}

void testeComunicacaoEsp8266(){
  //testando comunicação com o módulo WiFi
  monitor.flush();
  monitor.println("AT");
  delay(2000);
    
  if(monitor.find("OK")){
    Serial.println("Comunicacao com modulo ESP8266: OK");
  }
  else {
    Serial.println("Erro no modulo ESP8266");
  }
}

boolean conectarWIFI(){
  Serial.println("Conectando a WiFi...");
  String cmd ="AT+CWMODE=1";
  monitor.println(cmd);
  delay(2000);
  monitor.flush(); //clear buffer
  cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASSWORD;
  cmd+="\"";
  monitor.println(cmd);
  delay(5000);
  
  if(monitor.find("OK")){
    Serial.println("Conectado com sucesso!");
    return true;
  }else{
    Serial.println("Falha na conexao!");
    return false;
  }
  Serial.println();
}


