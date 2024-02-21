/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/21 下午 04:06
 */

#ifndef TEMPERATURE_HUMIDITY_SENSOR_H
#define TEMPERATURE_HUMIDITY_SENSOR_H

#include <DHT.h>

/**
 * 温湿度传感器类
 * 使用单例模式
 */
class TemperatureHumiditySensor {
private:
    DHT dht;
    static TemperatureHumiditySensor *instance;

    // 私有构造函数
    TemperatureHumiditySensor(int pin, int type);

public:
    // 禁止复制构造函数和赋值操作
    TemperatureHumiditySensor(const TemperatureHumiditySensor &) = delete;

    TemperatureHumiditySensor &operator=(const TemperatureHumiditySensor &) = delete;

    // 获取单例实例的方法
    static TemperatureHumiditySensor *getInstance(int pin, int type);

    // 读取湿度
    float readHumidity();

    // 读取温度
    float readTemperature();
};

#endif // TEMPERATURE_HUMIDITY_SENSOR_H
