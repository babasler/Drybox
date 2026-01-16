#include <DHT22.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "mqttwrapper.h"
#include "battery.h"
#include "secret.h"

#define DEBUG

#ifdef DEBUG
#define DBG(x) Serial.println(x)
#else
#define DBG(x) do {} while (0)
#endif

#define pinDATA 17 
#define DRYBOX_STATE_TOPIC "drybox/state"
#define MAX_ATTEMPTS 5
#define SLEEP_BETWEEN_PUBS 600 //10 Min

char* mqtt_server = "neptune4";
int mqtt_port = 1883;
char* mqtt_clientId = "drybox_1";

MqttWrapper mqtt(mqtt_server, mqtt_port, mqtt_clientId);

struct Measurement {
  float humidity;
  float temperature;
  float battery_level;
};

DHT22 dht22(pinDATA);

void mqttPublishState(){
   if(!mqtt.connect(MAX_ATTEMPTS)){
      DBG("MQTT Connection failed");
   } else {
      if (dht22.getLastError() != dht22.OK) {
          DBG("last error :");
          DBG(dht22.getLastError());
          return;
      }   
      Measurement m;
      m.temperature = round(dht22.getTemperature() * 10.0) / 10.0;
      m.humidity = round(dht22.getHumidity() * 10.0) / 10.0;
      //m.battery_level = round(getVbatt() * 10.0) / 10.0;
      m.battery_level = 15.0;
      StaticJsonDocument<200> doc;
      // Werte setzen
      doc["id"] = "1";
      doc["temperature"] = m.temperature;
      doc["humidity"] = m.humidity;
      doc["battery_level"] = m.battery_level;
      // Serialisieren in Char-Array
      char payload[200];
      serializeJson(doc, payload);  
      mqtt.publish(DRYBOX_STATE_TOPIC, payload);
      DBG("Published MQTT State:");
      DBG(payload);
      mqtt.disconnect();
   }
}

void goToSleep(uint64_t sleepTimeSec) {
  DBG("Going to sleep now");
  esp_sleep_enable_timer_wakeup(sleepTimeSec * 1000000ULL);
  esp_deep_sleep_start();
}
 
void setup() {
  Serial.begin(115200);
   
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(100);
  }

  if(WiFi.status() == WL_CONNECTED){
    delay(500);
    DBG("WIFI OK");
    mqttPublishState();
  }
  else {
    DBG("WIFI NOT OK");
  }

  vTaskDelay(pdMS_TO_TICKS(5000));
  
  //goToSleep(SLEEP_BETWEEN_PUBS);

}

void loop() {
  mqttPublishState();
  vTaskDelay(pdMS_TO_TICKS(15 * 60 * 1000));
}
