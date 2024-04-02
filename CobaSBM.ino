//kode modofikasi Barbara & Rani - Project UTS SBM

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // library untuk mengolah JSON
#include <DHT.h>

const int DHT_PIN = 26;
const char* ssid = "ranipretty"; //  wifi ssid
const char* password = "1234NEAA";
const char* mqtt_server = "192.168.131.127"; // mosquitto server url

DHT sensor_dht(DHT_PIN, DHT22);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
float temp = 0;
float hum = 0;

StaticJsonDocument<100> doc;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Wifi BERHASIL TERKONEKSI ke : ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi berhasil terkoneksi");
  Serial.print("Alamat IP : ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Baru melakukan koneksi MQTT ...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // pinMode(2, OUTPUT);
  Serial.begin(115200);
  sensor_dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
        reconnect();
      }
    client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float temp = sensor_dht.readTemperature();
    float hum = sensor_dht.readHumidity();

    StaticJsonDocument<200> doc;
    doc["Temperature"] = temp;
    doc["Humidity"] = hum;

    char jsonString[100];
    serializeJson(doc, jsonString);

    client.publish("/Barbara_sensor/data", jsonString);

    Serial.println("Data Terkirim:");
    Serial.println(jsonString);
  }
delay(2000);
}


