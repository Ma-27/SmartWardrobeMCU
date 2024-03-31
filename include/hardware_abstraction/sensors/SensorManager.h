/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:25
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#undef min
#undef max

#include <Arduino.h>
#include "DHT.h"
#include "utility/ProjectConfig.h" // 引入配置类
#include "TemperatureHumiditySensor.h"
#include "LightSensor.h"
#include "hardware_abstraction/sensors/PotentiometerSensor.h"

/**
 * **传感器管理器** (`SensorManager`)
    - 负责管理所有传感器，如DHT11。
    - 提供统一的接口供核心控制单元查询环境数据。
 */

class SensorManager : public CommandListener {
private:
    static SensorManager *instance;

    // 私有构造函数
    SensorManager();

    // 数据管理器实例
    DataManager *dataManager;

    // 温湿度传感器实例
    TemperatureHumiditySensor *tempHumSensor;

    // 光照传感器实例
    LightSensor *lightSensor;

    // 电位器传感器实例
    PotentiometerSensor *potentiometerSensor;

public:
    // 禁止复制构造函数和赋值操作
    SensorManager(const SensorManager &) = delete;

    SensorManager &operator=(const SensorManager &) = delete;

    // 获取单例实例的方法
    static SensorManager *getInstance();

    // 读取湿度
    float readHumidity();

    // 读取温度
    float readTemperature();

    // 读取光照强度
    int updateLightIntensity();

    // 读取电位器值
    int updatePotentiometerValue();


    /**根据光照强度和可变电阻值推荐理想的灯光强度
    *
    * 推荐光线算法规则：
    * 如果衣柜关闭，即感受到的光照强度小于15%,那么认为衣柜没有打开门。衣柜不开启辅助灯光。如果可调电阻值为0，则不开启灯光
    * 衣柜开启后，根据可调电阻和光照情况调整光线。根据可调电阻的权值和光照情况的权值6:4来调整。
    * 也就是说，光照强度越小，灯光越明亮；可调电阻值越大，灯光越明亮；用户能够调整的范围为60%。
    *
    * @param lightIntensity 现在的光照强度
    * @param potValue 用户输入的可变电阻值
    */
    int recommendLightIntensity(int lightIntensity, int potValue);

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // SENSOR_MANAGER_H


