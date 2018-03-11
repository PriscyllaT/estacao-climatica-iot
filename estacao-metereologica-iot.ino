/*
  estacao-metereologica-iot

  Autora: Priscylla Tavares
  Descricao: Projeto que pretende fazer a leitura de sensor de umidade,
  temperatura e chuva, e envia esses dados para serviço através do
  protocolo MQTT. 
  Utiliza a placa Arduino Uno, sensor DHT11 e sensor de chuva.
*/

#include <DHT.h>
#define DHTPIN 2 //pino digital para o sensor de umidade e temperatura
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Sataus: DHT11 funcionando");
  dht.begin();
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
