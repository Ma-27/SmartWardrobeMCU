/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/21 下午 04:06
 */

#include "hardware_abstraction/sensors/TemperatureHumiditySensor.h"


TemperatureHumiditySensor *TemperatureHumiditySensor::instance = nullptr;

// 获取单例实例的方法
TemperatureHumiditySensor *TemperatureHumiditySensor::getInstance(int pin, int type) {
    if (instance == nullptr) {
        instance = new TemperatureHumiditySensor(pin, type);
    }
    return instance;
}

// 私有构造函数
TemperatureHumiditySensor::TemperatureHumiditySensor(int pin, int type) : dht(pin, type) {
    // 初始化DHT传感器
    dht.begin();
}

// 读取湿度
float TemperatureHumiditySensor::readHumidity() {
    return dht.readHumidity();
}

// 读取温度
float TemperatureHumiditySensor::readTemperature() {
    return dht.readTemperature();
}

