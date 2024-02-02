#ifndef EnvironmentMonitor_h
#define EnvironmentMonitor_h

#include "Arduino.h"
#include "DHT.h"

class EnvironmentMonitor {
public:
    EnvironmentMonitor(uint8_t pin, uint8_t type);

    void begin();

    float readHumidity();

    float readTemperature();

private:
    DHT dht;
};

#endif
