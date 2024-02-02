#include "EnvironmentMonitor.h"

EnvironmentMonitor::EnvironmentMonitor(uint8_t pin, uint8_t type) : dht(pin, type) {}

void EnvironmentMonitor::begin() {
    dht.begin();
}

float EnvironmentMonitor::readHumidity() {
    // 读取湿度值并返回
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        Serial.println("Failed to read humidity from DHT sensor!");
        return -1.0;  // 返回-1.0表示读取失败
    }
    return humidity;
}

float EnvironmentMonitor::readTemperature() {
    // 读取温度值并返回
    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
        Serial.println("Failed to read temperature from DHT sensor!");
        return -1.0;  // 返回-1.0表示读取失败
    }
    return temperature;
}
