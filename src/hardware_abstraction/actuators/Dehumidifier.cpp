/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/29 下午 05:23
 */

// 实现文件
#include "hardware_abstraction/actuators/Dehumidifier.h"

// 初始化静态实例指针
Dehumidifier *Dehumidifier::instance = nullptr;

// 提供一个公共的访问方法
Dehumidifier *Dehumidifier::getInstance() {
    if (instance == nullptr) {
        instance = new Dehumidifier();
        instance->initDehumidifier();
    }
    return instance;
}

// 默认构造函数
Dehumidifier::Dehumidifier() {
    // 初始化时保持为空
}

void Dehumidifier::initDehumidifier() {
    // 设置降湿器的数字引脚
    pinMode(ProjectConfig::HUMID_DECREASE_CONTROL_PIN, OUTPUT);
}

// 控制降湿器开启
void Dehumidifier::turnOn() {
    // 设置降湿器的数字引脚为高电平
    digitalWrite(ProjectConfig::HUMID_DECREASE_CONTROL_PIN, HIGH);
    isOn = true;
}

// 控制降湿器关闭
void Dehumidifier::turnOff() {
    // 设置降湿器的数字引脚为低电平
    digitalWrite(ProjectConfig::HUMID_DECREASE_CONTROL_PIN, LOW);
    isOn = false;
}

// 直接调速
void Dehumidifier::speedControl(int speed) {
    // 同步速度
    this->speed = speed;
    analogWrite(ProjectConfig::HUMID_DECREASE_CONTROL_PIN, speed);
}

bool Dehumidifier::parseCommand(const String &command) {
    String trimmedCommand = command;
    trimmedCommand.trim();

    if (trimmedCommand.length() > 0) {
        return dispatchCommand(trimmedCommand, "", this);
    } else {
        return false;
    }
}

// 具体解析是哪个负责执行命令，派发给相应的监听器
bool Dehumidifier::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    command.trim();
    String processedCommand = command.substring(1);
    processedCommand.trim();

    if (processedCommand.startsWith("on")) {
        turnOn();
    } else if (processedCommand.startsWith("off")) {
        turnOff();
    } else {
        Serial.println("Unknown command in Dehumidifier: " + processedCommand);
        return false;
    }
    return true;
}
