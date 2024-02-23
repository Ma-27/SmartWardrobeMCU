/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/23 上午 09:16
 */

#include "hardware_abstraction/sensors/PotentiometerSensor.h"
#include "utility/ProjectConfig.h"

// 初始化静态成员变量
PotentiometerSensor *PotentiometerSensor::instance = nullptr;
const int PotentiometerSensor::potentiometerPin = ProjectConfig::potentiometerPin;

// 获取类的实例
PotentiometerSensor *PotentiometerSensor::getInstance() {
    if (instance == nullptr) {
        instance = new PotentiometerSensor();
    }
    return instance;
}

// 私有构造函数
PotentiometerSensor::PotentiometerSensor() {
    // 初始化Arduino引脚模式，如果需要
    pinMode(potentiometerPin, INPUT);
}

// 读取电位器的值并映射到0-100
int PotentiometerSensor::readValue() {
    int value = analogRead(potentiometerPin);
    // 将读取的值（0-1023）映射到0-100
    return map(value, 0, 1023, 0, 100);
}

