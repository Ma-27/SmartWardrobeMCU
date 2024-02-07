//
// Created by Mamh on 2024/2/2.
//

#ifndef HARDWARE_ABSTRACTION_H
#define HARDWARE_ABSTRACTION_H

// 确保包含DisplayManager的定义
#include "hardware_abstraction/display/DisplayManager.h"
#include "hardware_abstraction/sensors/SensorManager.h"
#include "data/pub-sub/Subscriber.h"
#include "data/pub-sub/EventManager.h"

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


    DisplayManager* displayManager; // 用于存储DisplayManager实例的指针
    SensorManager* sensorManager;  // 添加对SensorManager的引用

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    HardwareAbstraction(const HardwareAbstraction &) = delete;

    void operator=(const HardwareAbstraction &) = delete;

    // 提供一个公共的访问方法
    static HardwareAbstraction *getInstance();


    // // 采集温湿度并且显示在LCD屏幕上
    void processTemperatureAndHumidity(bool enabled);

    /**
   * 实现Subscriber接口要求的update方法。
   * 更新网络连接状态到LCD屏幕上。
   * @param message 收到的消息（收到它的子类的消息，int类型号）
   */
    void update(const Message &message) override;

    void initHAL();
};

#endif // HARDWARE_ABSTRACTION_H


