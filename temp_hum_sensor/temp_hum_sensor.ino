#include "DHTesp.h"
#include "ESP8266WiFi.h"
#include "PubSubClient.h"

const char* sensor_name = "[SENSOR_NAME]";
const char* temperature_topic = "sensor/temp/[room]";
const char* humidity_topic = "sensor/hum/[room]";

const int deep_sleep_dur = 3e8;

const char* SSID = "[WIFI_SSID]";
const char* PSK = "[WIFI_PASSWORD]";

const char* MQTT_BROKER = "{MQTT_HOST]";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "[MQTT_USERNAME]";
const char* MQTT_PASSWD = "[MQTT_PASSWORD]";

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;
 
void setup() {
    Serial.begin(9600);
    
    WiFi.begin(SSID, PSK);

    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
     
    Serial.println("Connected to the WiFi network");
    
    client.setServer(MQTT_BROKER, MQTT_PORT);
    
    dht.setup(5, DHTesp::DHT22);
    connect();
    client.loop();

    publish();
   
    client.disconnect(); // Disconnect from server to prevent server errors due to unclosed connections
    delay(1000);  // Sleep short duration to prevent errors
    ESP.deepSleep(deep_sleep_dur);
}

void publish() {
    float h = dht.getHumidity();
    float t = dht.getTemperature();

    char temp[8]; // Buffer big enough for 7-character float
    dtostrf(t, 4, 1, temp);
    char hum[8]; // Buffer big enough for 7-character float
    dtostrf(h, 4, 1, hum);
    
    client.publish(temperature_topic, temp);
    client.publish(humidity_topic, hum);
    Serial.print("published temperature and humidity");
}

void connect() {
    Serial.println("Connecting to MQTT Broker...");
    
    while (!client.connected()) {
      if (client.connect(sensor_name, MQTT_USER, MQTT_PASSWD )) {
          Serial.println("connected");  
      } else {
          Serial.print("failed with state ");
          Serial.println(client.state());
          delay(2000);
      }
    }
}

void loop() {
}
