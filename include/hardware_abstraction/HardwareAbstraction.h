/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 08:37
 */

#ifndef HARDWARE_ABSTRACTION_H
#define HARDWARE_ABSTRACTION_H

// 确保包含DisplayManager的定义
#include "hardware_abstraction/display/DisplayManager.h"
#include "hardware_abstraction/sensors/SensorManager.h"
#include "data/pub-sub/Subscriber.h"
#include "data/pub-sub/EventManager.h"
#include "hardware_abstraction/actuators/ActuatorManager.h"

/**
 * 硬件抽象层，负责提供硬件操作的统一接口，包括LCD显示等。
 */
class HardwareAbstraction : public Subscriber {
private:
    static HardwareAbstraction *instance; // 静态公共私有实例指针
    // 私有化构造函数
    HardwareAbstraction();

    // 保存事件接收器的一个对象，为了订阅并且接收网络更新的信息。
    EventManager *eventManager;
    // 保存数据管理器的一个对象，为了CURD传感器数据。
    DataManager *dataManager;

    // HAL负责管理的三大部件
    // 用于存储DisplayManager实例的指针
    DisplayManager *displayManager;
    // 添加对SensorManager的引用
    SensorManager *sensorManager;
    // 添加对ActuatorManager的引用
    ActuatorManager *actuatorManager;


    /**
    * 实现Subscriber接口要求的update方法。
    * 一旦任务调度器准备好了，就将更新温湿度等传感器的任务添加到队列。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;

    // 采集温湿度并且显示在LCD屏幕上
    void processTemperatureAndHumidity(bool enabled);

    // 采集光照强度和用户输入，针对用户输出综合判断是否开启灯
    void processLight(bool enabled);

    // 初始化硬件抽象层
    void initHAL();

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    HardwareAbstraction(const HardwareAbstraction &) = delete;

    void operator=(const HardwareAbstraction &) = delete;

    // 提供一个公共的访问方法
    static HardwareAbstraction *getInstance();
};

#endif // HARDWARE_ABSTRACTION_H


