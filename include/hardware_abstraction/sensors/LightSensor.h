/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/22 下午 03:45
 */

#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include <Arduino.h>
#include "data/DataManager.h"

// LightSensor类定义
class LightSensor {
public:
    // 获取LightSensor类的唯一实例
    static LightSensor *getInstance();

    // 读取光照强度
    int readLightIntensity();

private:
    // 私有构造函数（单例模式）
    LightSensor();

    // 禁止复制和赋值操作
    LightSensor(LightSensor const &) = delete;

    void operator=(LightSensor const &) = delete;

    // 唯一实例
    static LightSensor *instance;

    // 数据管理器实例
    DataManager *dataManager;

    // 光敏电阻连接的模拟输入引脚
    int lightSensorPin;

    // 将光敏电阻的值映射到光照强度
    int mapLightIntensity(int sensorValue);
};

#endif // LIGHTSENSOR_H