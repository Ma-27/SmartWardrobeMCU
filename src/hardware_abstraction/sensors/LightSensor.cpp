/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/22 下午 03:45
 */

#include "hardware_abstraction/sensors/LightSensor.h"
#include "utility/ProjectConfig.h"

LightSensor *LightSensor::instance = nullptr;

// 获取单例实例的方法
LightSensor *LightSensor::getInstance() {
    if (instance == nullptr) {
        instance = new LightSensor();
    }
    return instance;
}

// 私有构造函数（单例模式）
LightSensor::LightSensor() {
    lightSensorPin = ProjectConfig::lightSensorPin;
    pinMode(lightSensorPin, INPUT);
}


// 读取光照强度
int LightSensor::readLightIntensity() {
    int sensorValue = analogRead(lightSensorPin);
    return mapLightIntensity(sensorValue);
}

// 将光敏电阻的值映射到光照强度
int LightSensor::mapLightIntensity(int sensorValue) {
    // 这里假设光照强度是线性映射到0-100的值
    // 可以根据实际情况调整映射关系
    return map(sensorValue, 0, 1023, 0, 100);
}

