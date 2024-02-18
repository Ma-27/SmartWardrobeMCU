//
// Created by Mamh on 2024/2/2.
//

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "DHT.h"
#include "utility/ProjectConfig.h" // 引入配置类

/**
 * **传感器管理器** (`SensorManager`)
    - 负责管理所有传感器，如DHT11。
    - 提供统一的接口供核心控制单元查询环境数据。
 */

class SensorManager {
private:
    DHT dht;
    static SensorManager* instance;

    // 私有构造函数
    SensorManager(int pin, int type) : dht(pin, type) {
        // init the dht sensor.
        dht.begin();
    }

public:
    // 禁止复制构造函数和赋值操作
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;

    // 获取单例实例的方法
    static SensorManager* getInstance();

    float readHumidity();
    float readTemperature();
};

#endif // SENSOR_MANAGER_H


