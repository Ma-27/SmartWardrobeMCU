/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/30 上午 10:34
 */

#ifndef HEATER_H
#define HEATER_H

#include <Arduino.h>
#include "utility/ProjectConfig.h"
#include "core/CommandListener.h"

// 加热器类
class Heater : public CommandListener {
private:
    // 静态实例指针
    static Heater *instance;

    // 私有化构造函数
    Heater();

    // 初始化加热器的真正函数
    void initHeater();

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    Heater(const Heater &) = delete;
    void operator=(const Heater &) = delete;

    // 提供一个公共的访问方法
    static Heater *getInstance();

    // 控制加热器开启
    void turnOn();

    // 控制加热器关闭
    void turnOff();

    // 加热器是否开启
    bool isOn = false;

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // HEATER_H
