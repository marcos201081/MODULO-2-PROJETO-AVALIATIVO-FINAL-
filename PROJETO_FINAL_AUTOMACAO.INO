//---DECLARAÇÃO DE BIBLIOTECAS-----------------------------------------------------------------
#include <ArduinoJson.h>
#include <EspMQTTClient.h>
#include <DHT.h>


//---CRIAR VARIAVEIS DO MODULO DE ALARME-----------------------------------------------------

int sensor = 13;
String alarme;
int buzzer = 12;

//---VARIAVEL ---------------------------------------------------------------------------------
int lampada = 15;

//---DECLARAÇÃO DO DHT11 , CRIAR OBJETO DHT E VARIAVEIS -----------------------------------------
#define  DHTTYPE DHT11
int pinDHT = 5;
DHT dht (pinDHT, DHTTYPE);
float temperatura;
float umidade;

//---VARIAVEL DO MQTT--------------------------------------------------------------------------
char payload[100];
char temperaturaJson [100];
char umidadeJson     [100];
char alarmeJson      [100];

//------------geração do objeto EspMQTTClient------------------------
EspMQTTClient Client ("Içara Mais Tec_2G", "Espaco_4.0", "mqtt.tago.io", "Default", "9609700e-46a9-4dd5-9876-019c78433805", "Esp32", 1883);

void onConnectionEstablished() {
  Serial.println("conectado");

  Client.subscribe("info/led", [](const String &payload) {

    Serial.println(payload);

    if (payload == "L") {
      Serial.println("LED LIGADO");
      digitalWrite(lampada, HIGH);
    }
    if (payload == "D") {
      Serial.println("LED DESLIGADO");
      digitalWrite(lampada, LOW);
    }

  });
}

void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(lampada, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(sensor, INPUT );

}
void loop() {
  Temperatura();
  Alarme() ;

  delay(2000);

  Client.loop();

}
void Temperatura()
{
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
  Serial.print(temperatura);
  Serial.println("C");
  Serial.print(umidade);
  Serial.println("%");

  StaticJsonDocument<300> dadosTemperatura;
  StaticJsonDocument<300> dadosUmidade;

  dadosTemperatura["variable"] = "dados_da_temperatura";
  dadosTemperatura["value"]    = temperatura;


  dadosUmidade["variable"] = "dados_da_umidade";
  dadosUmidade["value"] = umidade;

  serializeJson(dadosTemperatura, temperaturaJson);
  serializeJson(dadosUmidade, umidadeJson);

  Client.publish("info/temperaturaJson", temperaturaJson);
  Client.publish("info/umidadeJson", umidadeJson);


}
void Alarme() {

  if (digitalRead(sensor) == LOW) {
    digitalWrite(buzzer, HIGH);
    Serial.println("ATENÇÃO! CASA INVADIDA");

    alarme = "ATENÇÃO! CASA INVADIDA";
  }
  else {
    digitalWrite(buzzer, LOW);
    Serial.println("ESTÁ TUDO DENTRO DA NORMALIDADE");
    alarme = "ESTÁ TUDO DENTRO DA NORMALIDADE";
  }

  StaticJsonDocument<300> dadosalarme;

  dadosalarme["variable"] = "dados_do_alarme";
  dadosalarme["value"]    = alarme;

  serializeJson(dadosalarme, alarmeJson);

  Client.publish("info/alarmeJson", alarmeJson);
}
