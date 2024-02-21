/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:25
 */

#include "hardware_abstraction/sensors/SensorManager.h"

SensorManager* SensorManager::instance = nullptr;

// 获取单例实例的方法
SensorManager* SensorManager::getInstance() {
    if (instance == nullptr) {
        instance = new SensorManager();
    }
    return instance;
}

// 私有构造函数
SensorManager::SensorManager() {
    // 初始化温湿度传感器实例
    tempHumSensor = TemperatureHumiditySensor::getInstance(ProjectConfig::DHTPIN, ProjectConfig::DHTTYPE);
}

// 读取湿度
float SensorManager::readHumidity() {
    // 使用温湿度传感器实例读取湿度
    return tempHumSensor->readHumidity();
}

// 读取温度
float SensorManager::readTemperature() {
    // 使用温湿度传感器实例读取温度
    return tempHumSensor->readTemperature();
}

