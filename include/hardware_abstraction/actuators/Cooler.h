/**
 * @description: 冷却器控制类, 用来控制冷却风扇的开关和速度. 使用数字 IO, 高电平开启, 低电平关闭.
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/29 下午 11:42
 */

#ifndef COOLER_H
#define COOLER_H

#include <Arduino.h>
#include "utility/ProjectConfig.h"
#include "core/CommandListener.h"

// 冷却器类
class Cooler : public CommandListener {
private:
    static Cooler *instance;  // 静态实例指针

    // 私有化构造函数
    Cooler();

    // 初始化冷却器的函数
    void initCooler();

    // 冷却风扇的转速
    int speed = 0;

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    Cooler(const Cooler &) = delete;
    void operator=(const Cooler &) = delete;

    // 提供一个公共的访问方法
    static Cooler *getInstance();

    // 控制冷却器开启
    void turnOn();

    // 控制冷却器关闭
    void turnOff();

    // 冷却器是否开启
    bool isOn = false;

    // 获取风扇转速
    int getFanSpeed() {
        return speed;
    }

    // 设置风扇转速
    void setFanSpeed(int Speed){
        this->speed = Speed;
    }

    // 通过PWM控制风扇转速，模拟连续调速
    void speedControl(int speed);

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // COOLER_H
