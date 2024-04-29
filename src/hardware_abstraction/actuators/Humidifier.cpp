/**
 * @description: 加湿控制类，用于控制喷雾加湿模块开始工作。使用数字 IO，高电平工作，低电平不工作.
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/29 下午 03:55
 */

#include "hardware_abstraction/actuators/Humidifier.h"

// 初始化静态实例指针
Humidifier *Humidifier::instance = nullptr;

// 提供一个公共的访问方法
Humidifier *Humidifier::getInstance() {
    if (instance == nullptr) {
        instance = new Humidifier();
        instance->initHumidifier();
    }
    return instance;
}

// 初始化所有执行器组件，这是该类的默认构造函数
Humidifier::Humidifier() {
    // KEEP IT EMPTY
}

void Humidifier::initHumidifier() {
    // 设置加湿器的数字引脚
    pinMode(ProjectConfig::HUMID_INCREASE_CONTROL_PIN, OUTPUT);
}

// 控制加湿喷雾器开启
void Humidifier::turnOn() {
    // 设置加湿器的数字引脚为高电平
    digitalWrite(ProjectConfig::HUMID_INCREASE_CONTROL_PIN, HIGH);
}

void Humidifier::turnOff() {
    // 设置加湿器的数字引脚为低电平
    digitalWrite(ProjectConfig::HUMID_INCREASE_CONTROL_PIN, LOW);
}


// 解析命令。TODO 注意这是最底层的命令解析了。这里只考虑了处理参数和执行命令，没有考虑命令的传递。
bool Humidifier::parseCommand(const String &command) {
    // 移除字符串首尾的空白字符
    String trimmedCommand = command;
    trimmedCommand.trim();

    if (trimmedCommand.length() > 0) {
        // 如果命令不为空（即含有子层级的命令），递交给其子类处理
        return dispatchCommand(trimmedCommand, "", this);
    } else {
        // 如果命令为空，返回false
        return false;
    }
}

// 具体解析是哪个负责执行命令，派发给相应的监听器
bool Humidifier::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 删除命令前的所有空格
    command.trim();

    // 去掉命令开头的"-"，以保留"-on"的格式
    String processedCommand = command.substring(1);
    processedCommand.trim();


    //  执行命令并且处理参数
    if (processedCommand.startsWith("on")) {
        // 设置打开加湿器
        turnOn();
        isOn = true;
    } else if (processedCommand.startsWith("off")) {
        // 设置打开加湿器
        turnOff();
        isOn = false;
    } else {
        // 不继续向下处理
        Serial.println("Unknown command in Humidifier: " + processedCommand);
        return false;
    }
    return false;
}