# Latest changes
*20221130*
  - Readme and [example](#minimalistic-example) updated.
  - Additional (simplified) version to use the [on()-method](#improved-callback-handling) to subscribe to topics.

# Arduino ESP8266/ESP32 Client for MQTT using PubSubClient-Library

This library provides a client for doing simple publish/subscribe messaging with
a server that supports MQTT. It wraps the functionality of the PubSubClient-Library
by deriving a wrapper class from PubSubClient class.

The main additions to the base class are:
 * the [on()-method](#improved-callback-handling) that allows a link of a specific topic to a specific callback 
   (i. e. yoy no longer have to parse the incoming topics to link them to specific actions in your sketch).
 * The connection/reconnection is handled whithin the `loop()` method. That means you no longer need
    to wait for WiFi-layer to be established before connecting to the MQTT-Server or subscribing to topics. 

## Basic usage and deviations to PubSubClient

 * Instead of "PubSubClient.h" include "ESPPubSubClientWrapper.h"
 * Instead of class `PubSubClient` instantiate a client Object from class `ESPPubSubClientWrapper`
 * There are only two ways of constructing an `ESPPubSubClientWrapper` object:
	* `ESPPubSubClientWrapper clientName(const char *domain, uint16_t port)` to domain name of
	    host as in i. e. "broker.mqtt-dashboard.com"
	*  `ESPPubSubClientWrapper(uint8_t *ipaddr, uint16_t port)` to specify the IP4-address of 
		the host as in i. e. {192, 168, 4, 1}
	* in both cases the parameter port can be omitted and defaults to 1883.
 * The methods `setServer()`, `setClient()` and `setStream()` must not be used.
 * The methods `setCallback()` and `subscribe()` can be used as usual, but using the method `on()` is
   often the better choice as it allows a direct link of a specific topic to a specific callback.
 * You can use callbacks to be informed on certain connection events:
	* `ESPPubSubClientWrapper& onConnect(std::function<void(uint16_t)> connectCallback)`: `void connectCallback(uint16_t count)`
		is called upon successful connect. Parameter `count` is zero for first successful connection and increased for each
		reconnect.
	* `ESPPubSubClientWrapper& onDisconnect(std::function<void(uint16_t)> disconnectCallback)`: 
		`void disconnectCallback(uint16_t count)` is called if connection to MQTT server is lost. Parameter `count` is One on first
        disconnect and will be increased for each further disconnect.
    *  `ESPPubSubClientWrapper& onConnectFail(std::function<bool(uint16_t)> connectFailCallback)`:
		`bool connectFailCallback(uint16_t count)` is called if connection to MQTT server can not be established even if WiFi
		is working. Parameter `count` is One on first fail and will be increased for each further fail. If the callback returns
		`true` (or is not set at all) a next connection attempt will be made after a 5 seconds timeout.
 * `loop()` always returns true. Use the three callbacks above or call `connected()` to evaluate the state of MQTT connection.	
 * It is not necessary to call any `connect()` method. In that case connection will be done with random client-id.
 * A call to any `connect()` method will not actually connect but just set the parameters for later connection attempt.
 * The connection attempt will be made during the `loop()` method once the WiFi-connection is established (i. e. no need
   to wait for the WiFi connection to be established before connecting the MQTT-client).
 * If connection is lost it will automatically being reestablished during `loop()` including all subscriptions made by using
   either the `on()` or `subscribe()` method.

## Improved callback handling

By default, you can use only one central callback to react on incoming messages using the `subscribe()` method. If you 
subscribe to multiple topics, you have to check each incoming topic and select the appropriate reaction. This is 
simpilfied with the new `on()`-methods, which allow to link seperate callback functions to different topics.

There are two variants of the `on()`-method:
 * In the first variant, the API for the method `on()` is equivalent to the `subscribe()` method. 
   The parameters are the same:
	`ESPPubSubClientWrapper& on(const char* topic, MQTT_CALLBACK_SIGNATURE, uint8_t qos = 0);` 
	- in this case the callback needs to be a function as defined by `MQTT_CALLBACK_SIGNATURE`, which is a function of 
		type `void callback(char *topic, uint8_t* payload, unsigned int payloadLen);`

 * As with MQTT the payload usally is plain text, that can be simplified by using the second variant for which the API
 	is defined as 
   	`ESPPubSubClientWrapper& on(const char* topic, MQTT_CALLBACK_SIGNATURE2, uint8_t qos = 0);`
	- in this case the callback needs to be a function as defined by `MQTT_CALLBACK_SIGNATURE2`, which is a function of 
		type `void callback(char *topic, char* payload);`
	- if no payload was sent with the message, `payload` will be equal to `NULL`, otherwise payload will be a standard 
	  `char *` (with terminating 0)

The following will apply for either variant:
 * the return value (which can be ignored) is a reference to the calling object thus allowing to chain multiple calls 
   to `on()`.
 * The topics used for calling `on()` can include the MQTT-Wildcards `#` and `+`.
 * There is no fixed limit on subscriptions that can be made using `on()`. In practice the number is limited by available 
   memory (RAM), as the topic for each subscriptions has to be stored as RAM copy. 
 * It is possible that an incoming topic matches to more than one callback. Consider as example `client.on("test/#", 
	callback1);client.on("#", callback2);`. The second topic subscription matches also topics that match the first call 
	to `on()`. In that case not all matching callbacks fire but the order of subscription is relevant. In the given example, 
    each incoming message that matches `"test/#"` will fire `callback1()` while all other topics are matched to `callback2()`.
 * The pointer to the payload is only valid during the callback. If you want to use it outside the scope of the callback
   function, you have to create a local copy for the application (same as for the `subscribe()`-method)
	

## Minimalistic Example

The following is a minimalistic example (for ESP8266). In this example two subscriptions are made to the topics `"hello"` and
`"world"`. If message `"hello"` is received the function `callbackHello()` will be called, if `"world"` is received 
`callbackWorld()` will be called.

`callbackHello()` uses the simplified API (the payload is a zero-terminated char string) `callbackWorld()` uses the 
standard approach with payload being represented by an `uint8_t` array and the payload length as additional parameter.

The functionality is the same in both cases (`payload` will be echoed to Serial monitor).


```
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
```


## Limitations

 * Usual limitations of PubSubClient library apply.
 * Do not use the set-methods established by base class except `setCallback()`. I. e. do not use
	* `PubSubClient& setServer(uint8_t * ip, uint16_t port);`
	* `PubSubClient& setServer(const char * domain, uint16_t port);`
	* `PubSubClient& setClient(Client& client);`
	* `PubSubClient& setStream(Stream& stream);`


## Compatible Hardware

 - ESP8266
 - ESP32

## License

This code is released under the MIT License.
