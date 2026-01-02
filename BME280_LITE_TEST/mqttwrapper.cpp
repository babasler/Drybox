#include "mqttwrapper.h"

MqttWrapper::MqttWrapper(char* server, int port, char* clientId) {
    this->server = server;
    this->port = port;
    this->clientId = clientId;
    client.setServer(server, port);
}

bool MqttWrapper::connect(uint8_t maxAttempts) {
    uint8_t attempts = 0;
    while(!client.connect(this->clientId) && attempts < maxAttempts) {
        attempts++;
        delay(100);
    }
    if (client.connected()) {
        return true;
    }
    DBG(client.state());
    return false;
}

bool MqttWrapper::publish(const char* topic, const char* payload) {
    if (client.connected()) {
        client.publish(topic, payload, true);
        for (int i = 0; i < 10; i++) {
            client.loop();
            delay(50);
        }
        return true;
    }
    else {
        return false;
    }
}

bool MqttWrapper::isConnected() {
    return client.connected();
}

void MqttWrapper::disconnect() {
    client.disconnect();
}