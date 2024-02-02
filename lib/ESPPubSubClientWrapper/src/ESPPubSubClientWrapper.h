#ifndef ESPPubSubClientWrapper_h
#define ESPPubSubClientWrapper_h
#include <PubSubClient.h>
#define QUEUE_CALLBACKS
#define PUBLISH_WAITCONNECTED
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <functional>
#define CALLBACK_SIGNATURE_VOID_UINT16 std::function<void(uint16_t)> 
#define CALLBACK_SIGNATURE_BOOL_UINT16 std::function<bool(uint16_t)> 
#define MQTT_CALLBACK_SIGNATURE2 std::function<void(char*, char*)> callback2

class onEventItem;
class PendingCallbackItem;
class WaitingPublishItem;



class ESPPubSubClientWrapper : public PubSubClient {
  public:
	ESPPubSubClientWrapper(const char *domain, uint16_t port = 1883);
	ESPPubSubClientWrapper(uint8_t *ipaddr, uint16_t port = 1883);
	/* 
		Any connect() always returns true, as it just stores the connect information for later (waiting for client connection).	
		To evaluate wether the actual connect attempt is successful, use callbacks definded by onConnect() or onConnectFail(). 

		Note that a call to connect() is not required (to simplify the application). 
		In that case, the connection is made with an random client-id.
	*/
	boolean connect(const char* id);
	boolean connect(const char* id, const char* user, const char* pass);
	boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
	boolean connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
	boolean connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage, boolean cleanSession);
	/*	
		Compatibility function. Each topic subscribed to by this function will use the "global" callback set by the
		setCallback() method.
		Better use on() for mapping specific topics to specific callbacks.
		Method can be called even if client is not (yet) connected, i. e. during setup() in Arduino. 
		Once the connection is (re-)established, all outstanding subscriptions will be resolved.
	*/
	boolean subscribe(const char* topic, uint8_t qos = 0);

	/* 
		unsubscribe() is used both for topics subscribed to by using the method subscribe() or the method on() 
	*/
	boolean unsubscribe(const char* topic);	
	
	
	/*
		Compatibility function. Sets the "global" callback for each topic subscribed to by the subscribe()-method.
		Better use on() for mapping specific callbacks to to specific callbacks.
	*/
	
	ESPPubSubClientWrapper& setCallback(MQTT_CALLBACK_SIGNATURE);
	
	/*
		Simplified API to attach a specific callbacks to a specific copy.
		As long as memory lasts, any number of matching pairs can be defined.
		If NULL is given as second parameter (the callback) the "global" callback will be used (in that case there is
		no difference to the subscribe()-method above).
		As with subscribe(), this method can be called even if the client is not (yet) connected. As mentioned before,
		once the connection is (re-)established, all outstanding subscriptions resulting from calls to the on()-method 
		will be resolved.
		
		The wildcards '#' and '?' are respected as per MQTT protocol definition.
		
		There is no checking of any kind if the subscriptions overlap. 
		I. e. if you subscribe to the topic "hello/#"  and link it to a callback 
			called helloAll(char*, uint8_t*, unsigned int) using on("hello/#", helloAll); 
		and you also subscribe to "hello/world" and link this to 
			helloWorld(char*, uint8_t*, unsigned int) using on("hello/world", helloWorld);

		then helloWorld() will only be called if the corresponding call to the method on("hello/world",...) is done 
		before the call to the method on("hello/#", ...) is done.
		(If it is like above, i. e. the call to on("hello/#",...) before on("hello/world", ...), the latter would never fire
		as the former already consumes all "hello/world" messages.)
		This order is also retained if the subscriptions are renewed after a successful reconnect.
		
		As a rule of thumb, define the most specific (detailed) topics first before going down to more generic ones.
	*/
	ESPPubSubClientWrapper& on(const char* topic, MQTT_CALLBACK_SIGNATURE, uint8_t qos = 0);
	ESPPubSubClientWrapper& on(const char* topic, MQTT_CALLBACK_SIGNATURE2, uint8_t qos = 0);
	ESPPubSubClientWrapper& onConnect(CALLBACK_SIGNATURE_VOID_UINT16 callback);
	ESPPubSubClientWrapper& onDisconnect(CALLBACK_SIGNATURE_VOID_UINT16 callback);
	ESPPubSubClientWrapper& onConnectFail(CALLBACK_SIGNATURE_BOOL_UINT16 callback);
	boolean publish_waitConnected(const char* topic, const char* payload);
	boolean publish_waitConnected(const char* topic, const char* payload, boolean retained);
	boolean publish_waitConnected(const char* topic, const uint8_t * payload, unsigned int plength);
	boolean publish_waitConnected(const char* topic, const uint8_t * payload, unsigned int plength, boolean retained);
	boolean loop();
  protected:
	void setState(int16_t stateNb);
    void receivedCallback(char* topic, byte* payload, unsigned int length); 
    void runState(int16_t stateNb);
	boolean doSetConnect(const char* id, const char* user, const char* pass, const char* willTopic, 
						uint8_t willQos, boolean willRetain, const char* willMessage, boolean cleanSession);
	CALLBACK_SIGNATURE_VOID_UINT16 _cbConnect = NULL;
	CALLBACK_SIGNATURE_VOID_UINT16 _cbDisc = NULL;
	CALLBACK_SIGNATURE_BOOL_UINT16 _cbConnectFail = NULL;
	MQTT_CALLBACK_SIGNATURE = NULL;
	uint16_t _connectFailCount, _discCount = 0;
	WiFiClient _wiFiClient;
	int16_t _stateNb = 0;
	uint32_t _stateStartTime = 0;
	bool _firstRetry = false;
//	int _subscribed = 0;
	char* _connect_id = NULL;
	char* _connect_user = NULL;
	char* _connect_pass = NULL;
	char* _connect_willTopic = NULL;
	uint8_t _connect_willQos = 0;
	bool _connect_willRetain = false;
	char* _connect_willMessage = NULL;
	bool _connect_cleanSession = true;

	onEventItem* _firstOnEvent = NULL;
	onEventItem* _lastOnEvent = NULL;
	onEventItem* _subsciptionPending = NULL;
#if defined(QUEUE_CALLBACKS)	
	PendingCallbackItem* _firstPendingCallback = NULL;
	PendingCallbackItem* _lastPendingCallback = NULL;
#endif	

#if defined(PUBLISH_WAITCONNECTED)
	WaitingPublishItem* _firstWaitingPublish = NULL;
	WaitingPublishItem* _lastWaitingPublish = NULL;	
#endif
};


#endif