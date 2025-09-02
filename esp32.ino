#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 15           // Pino do sensor DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define LDR_PIN 34          // Pino analógico onde está conectado o LDR

// Wi-Fi e ThingSpeak
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* apiKey = "0WU7J5JUOXK6HK3G"; // Sua chave de escrita
const char* server = "http://api.thingspeak.com";

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LDR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" conectado!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Falha ao ler o sensor DHT22!");
      delay(2000);
      return;
    }

    // Leitura da luminosidade
    int ldrValue = analogRead(LDR_PIN);  // valor entre 0 e 4095
    float luminosidade = map(ldrValue, 0, 4095, 0, 100); // converte para % (0 a 100%)

    // Mostra no serial monitor
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print(" °C, Umidade: ");
    Serial.print(humidity);
    Serial.print(" %, Luminosidade: ");
    Serial.print(luminosidade);
    Serial.println(" %");

    // Envio ao ThingSpeak
    HTTPClient http;
    String url = String(server) + "/update?api_key=" + apiKey +
                 "&field1=" + String(temperature, 2) +
                 "&field2=" + String(humidity, 2) +
                 "&field3=" + String(luminosidade, 2);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Dados enviados ao ThingSpeak com sucesso.");
      Serial.print("Código HTTP: ");
      Serial.println(httpCode);
      Serial.println("Resposta: ");
      Serial.println(payload);
    } else {
      Serial.print("Erro ao enviar dados. Código HTTP: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    Serial.println("WiFi não conectado. Tentando reconectar...");
    WiFi.begin(ssid, password);
  }

  delay(15000); // respeita limite do ThingSpeak
}
