#ifndef _MQTTWRAPPER_H_
#define _MQTTWRAPPER_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "secret.h"

#define DEBUG

#ifdef DEBUG
#define DBG(x) Serial.println(x)
#else
#define DBG(x) do {} while (0)
#endif

class MqttWrapper {
    public:
        MqttWrapper(char* server, int port, char* clientId);
        bool connect(uint8_t maxAttempts);
        bool publish(const char* topic, const char* payload);
        bool isConnected();
        void disconnect();
    private:
        WiFiClient espClient;
        PubSubClient client{espClient};
        char* server;
        int port;
        char* clientId;
};

#endif