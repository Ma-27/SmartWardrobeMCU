/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/23 上午 09:16
 */

#ifndef PotentiometerSensor_h
#define PotentiometerSensor_h

#include "Arduino.h"

class PotentiometerSensor {
public:
    // 获取类的实例
    static PotentiometerSensor *getInstance();

    // 读取电位器的值并映射到0-100
    int readValue();

private:
    // 私有化构造函数以实现单例模式
    PotentiometerSensor();

    // 私有化拷贝构造函数和赋值操作符以防止复制
    PotentiometerSensor(const PotentiometerSensor &) = delete;

    PotentiometerSensor &operator=(const PotentiometerSensor &) = delete;

    // 类的唯一实例
    static PotentiometerSensor *instance;

    // 用于读取电位器值的引脚编号，根据实际连接修改
    static const int potentiometerPin;
};

#endif
