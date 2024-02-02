/*
 ESPPubSubClientWrapper MQTT reconnect example

 This sketch demonstrates the simplified API for automatic reconnect to the MQTT broker once the connection is lost.

 It connects to an MQTT server then:
  - subscribes to the topic DISCONNECT_TOPIC. If a message with that topic is received, it disconnects the 
	client.
  - it should then reconnect to the MQTT-Broker.	
  - the onConnect() callback is used to print a message after successful (re-)connection.	
  
*/

#include <ESP8266WiFi.h>
#include <ESPPubSubClientWrapper.h>

// Update these with values suitable for your network.

const char* ssid = "..........";
const char* password = "..........";
const char* mqtt_server = "broker.mqtt-dashboard.com";
#define DISCONNECT_TOPIC "disconnect"

ESPPubSubClientWrapper client(mqtt_server);


void connectSuccess(uint16_t reConnectCount) {
  Serial.print("Connected to MQTT-Broker!\nThis is connection nb: ");
  Serial.println(reConnectCount + 1);
}

void gotDisconnected(uint16_t disconnectCount) {
  Serial.print("Got disconnected from MQTT-Broker!\nThis is disconnect nb: ");
  Serial.println(disconnectCount);
}

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  // No need to wait here for a successful connection, this is done via loop()	
  
  client.onConnect(connectSuccess)
	.onDisconnect(gotDisconnected)
	.on(DISCONNECT_TOPIC, [](char* topic, byte* payload, unsigned int length) {client.disconnect();});

}

void loop() {
  client.loop();
}
