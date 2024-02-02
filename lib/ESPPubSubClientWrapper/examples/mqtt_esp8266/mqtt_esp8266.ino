/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a nonblocking
 reconnect function built into the loop()-function. 

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <ESPPubSubClientWrapper.h>

// Update these with values suitable for your network.

const char* ssid = "...........";
const char* password = "..........";
const char* mqtt_server = "broker.mqtt-dashboard.com";

ESPPubSubClientWrapper client(mqtt_server);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void connectSuccess(uint16_t count) {
  Serial.println("Connected to MQTT-Broker!\nThis is connection nb: ");
  Serial.println(count);
  lastMsg = millis();
  client.publish("outTopic", "hello world");
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setCallback(callback);
  client.onConnect(connectSuccess);
  client.on("lightOff", [](char* topic, byte* payload, unsigned int length) {digitalWrite(BUILTIN_LED, HIGH);});
  client.on("lightOn", [](char* topic, byte* payload, unsigned int length) {digitalWrite(BUILTIN_LED, LOW);});
  client.on("disconnect", [](char* topic, byte* payload, unsigned int length) {client.disconnect();});
  client.subscribe("inTopic");
}

void loop() {

  client.loop();

  long now = millis();
  if (client.connected())
    if (now - lastMsg > 2000) {
      lastMsg = now;
      ++value;
      snprintf (msg, 50, "hello world #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
    }
}
