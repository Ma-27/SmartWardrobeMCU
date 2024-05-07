/**
 * @description: 加湿控制类，用于控制喷雾加湿模块开始工作。使用数字 IO，高电平工作，低电平不工作.
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/29 下午 03:55
 */

#ifndef HUMIDIFIER_H
#define HUMIDIFIER_H


#include <Arduino.h>
#include <Arduino.h>
#include "utility/ProjectConfig.h"
#include "core/CommandListener.h"



// 加湿喷雾类
class Humidifier : public CommandListener{
private:
    static Humidifier *instance;  // 静态实例指针

    // 私有化构造函数
    Humidifier();

    // 初始化加湿器的真正函数
    void initHumidifier();


public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    Humidifier(const Humidifier &) = delete;

    void operator=(const Humidifier &) = delete;

    // 提供一个公共的访问方法
    static Humidifier *getInstance();

    // 控制加湿喷雾器开启
    void turnOn();

    // 控制加湿喷雾器关闭
    void turnOff();

    // 加湿器是否开启
    bool isOn = false;

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // HUMIDIFIER_H