/**
 * @description: 冷却器控制类, 用来控制冷却风扇的开关和速度. 使用数字 IO, 高电平开启, 低电平关闭.
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/29 下午 11:42
 */

#include "hardware_abstraction/actuators/Cooler.h"

// 初始化静态实例指针, 确保类的单例模式
Cooler *Cooler::instance = nullptr;

// 提供一个公共的访问方法，保证全局只有一个冷却器实例
Cooler *Cooler::getInstance() {
    if (instance == nullptr) {
        // 如果实例不存在, 则新建一个
        instance = new Cooler();
        // 初始化冷却器设置
        instance->initCooler();
    }
    return instance;
}

// 默认构造函数，设置为私有以支持单例模式
Cooler::Cooler() {
    // 构造函数内容为空，初始化在 initCooler 中完成
}

void Cooler::initCooler() {
    // 设置冷却器控制引脚为输出模式
    pinMode(ProjectConfig::COOL_CONTROL_PIN, OUTPUT);
}

void Cooler::turnOn() {
    // 将冷却器控制引脚设为高电平，启动冷却器
    digitalWrite(ProjectConfig::COOL_CONTROL_PIN, HIGH);
    // 更新冷却器状态为开启
    isOn = true;
}

void Cooler::turnOff() {
    // 将冷却器控制引脚设为低电平，关闭冷却器
    digitalWrite(ProjectConfig::COOL_CONTROL_PIN, LOW);
    // 更新冷却器状态为关闭
    isOn = false;
}

void Cooler::speedControl(int speed) {
    // 设置 PWM 信号来调节风扇速度，针对控制算法提供不同的冷却效果
    this->speed = speed; // 更新内部速度设置
    // 输出 PWM 信号
    analogWrite(ProjectConfig::COOL_CONTROL_PIN, speed);
}

bool Cooler::parseCommand(const String &command) {
    // 去除命令字符串的前后空格
    String trimmedCommand = command;
    trimmedCommand.trim();

    if (trimmedCommand.length() > 0) {
        // 处理命令
        return dispatchCommand(trimmedCommand, "", this);
    } else {
        return false;
    }
}

bool Cooler::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 再次确保命令无额外空格
    command.trim();
    // 去除命令的第一个字符（是控制符"-"）
    String processedCommand = command.substring(1);
    // 去除处理后的命令字符串的前后空格
    processedCommand.trim();

    if (processedCommand.startsWith("on")) {
        // 如果命令是开启风扇
        turnOn();
    } else if (processedCommand.startsWith("off")) {
        // 如果命令是关闭风扇
        turnOff();
    } else {
        // 未知命令打印错误
        Serial.println("Unknown command in Cooler: " + processedCommand);
        return false;
    }
    return true;
}
