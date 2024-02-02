#include <ESP8266WiFi.h>
#include <ESPPubSubClientWrapper.h>

const char* ssid = "..........";				// Put your WiFi credentials here
const char* password = "..........";
const char* mqtt_server = "broker.mqtt-dashboard.com";

ESPPubSubClientWrapper client(mqtt_server);

  
/*
This function will be called if topic "hello" is received on MQTT and echo the payload on Serial monitor.
It uses the simplified API with payload being converted to a 0 terminated char pointer (or NULL if no payload was sent)
*/  
void callbackHello(char* topic, char * payload) {
  Serial.println("\r\nMessage ""hello"" received");
  if (payload)
  {
    Serial.printf("Payload-len=%d, Payload=\"%s\"\r\n", strlen(payload), payload);
  }
  else
    Serial.println("Payload is NULL.");
}

/*
This function will be called if topic "world" is received on MQTT and echo the payload on Serial monitor.
It uses the default API with payload as uint_8-array with valid length given by payloadLen (0, if no payload was sent)
*/  
void callbackWorld(char* topic, uint8_t* payload, unsigned int payloadLen) {
  Serial.println("\r\nMessage ""world"" received");
  if (payload)
  {
    char s[payloadLen + 1];
    memcpy(s, payload, payloadLen);
    s[payloadLen] = '\0';
    Serial.printf("Payload-len=%d, Payload=\"%s\"\r\n", payloadLen, s);
  }
  else
    Serial.println("Payload is NULL.");

}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  client.on("hello", callbackHello);
  client.on("world", callbackWorld);  
}

void loop() {
  client.loop();
}

