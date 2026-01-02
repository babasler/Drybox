#include <BME280_LITE.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "battery.h"
#include "mqttwrapper.h"

#define BME_ADDR 0x76
#define SDA_PIN 22
#define SCL_PIN 23
#define DRYBOX_STATE_TOPIC "drybox/state"
#define MAX_ATTEMPTS 5
#define SLEEP_BETWEEN_PUBS 600 //10 Min

#define DEBUG

#ifdef DEBUG
#define DBG(x) Serial.println(x)
#else
#define DBG(x) do {} while (0)
#endif

BME280_LITE bme;

char* mqtt_server = "neptune4";
int mqtt_port = 1883;
char* mqtt_clientId = "drybox_1";

MqttWrapper mqtt(mqtt_server, mqtt_port, mqtt_clientId);

struct Measurement {
  float humidity;
  float temperature;
  float battery_level;
};

void mqttPublishState(){
   if(!mqtt.connect(MAX_ATTEMPTS)){
      DBG("MQTT Connection failed");
   } else {
      Measurement m;
      BME_SensorData temperature = bme.readTemperature(BME_ADDR); 
      BME_SensorData humidity = bme.readHumidity(BME_ADDR);
      m.temperature = temperature.data;
      m.humidity = humidity.data;
      m.battery_level = getVbatt();
      
      char payload[100];      
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
  Serial.begin(9600);
  
  Wire.begin(SDA_PIN, SCL_PIN); 
  
  if (!bme.begin(&Wire,         
                 BME_ADDR, 
                 BME_H_X1, 
                 BME_T_X1, 
                 BME_P_X16, 
                 BME_NORMAL, 
                 BME_TSB_0_5MS, 
                 BME_FILTER_2)) {
    DBG("BME280 initialization failed!");
    while(1);
  }
  
  if (!bme.calibrate(BME_ADDR)) {
    DBG("BME280 calibration failed!");
    while(1);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(100);
  }

  if(WiFi.status() == WL_CONNECTED){
    delay(500);
  }

  mqttPublishState();
  
  goToSleep(SLEEP_BETWEEN_PUBS);

}

void loop() {

}
