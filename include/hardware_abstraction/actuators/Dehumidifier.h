/**
 * @description: 干燥器控制类, 用来控制干燥器和干燥风扇原件. 使用数字 IO, 高电平开启, 低电平关闭.
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/29 下午 05:23
 */

#ifndef DEHUMIDIFIER_H
#define DEHUMIDIFIER_H

#include <Arduino.h>
#include "utility/ProjectConfig.h"
#include "core/CommandListener.h"

// 降湿器类
class Dehumidifier : public CommandListener {
private:
    static Dehumidifier *instance;  // 静态实例指针

    // 私有化构造函数
    Dehumidifier();

    // 初始化降湿器的真正函数
    void initDehumidifier();

    // 电动机的转速
    int speed = 0;

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    Dehumidifier(const Dehumidifier &) = delete;
    void operator=(const Dehumidifier &) = delete;

    // 提供一个公共的访问方法
    static Dehumidifier *getInstance();

    // 控制降湿器开启
    void turnOn();

    // 控制降湿器关闭
    void turnOff();

    // 降湿器是否开启
    bool isOn = false;

    // 电动机转速
    int getFanSpeed() {
        return speed;
    }

    // 设置电动机转速
    int setFanSpeed(int Speed){
        this->speed = Speed;
    }

    // 通过PID控制算法控制转速，这里通过PWM端口控制占空比，近似连续的调速
    void speedControl(int speed);

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // DEHUMIDIFIER_H
