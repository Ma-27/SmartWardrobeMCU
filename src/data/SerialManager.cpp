//
// Created by Mamh on 2024/2/4.
//

#include "data/SerialManager.h"

SerialManager *SerialManager::instance = nullptr;

SerialManager *SerialManager::getInstance() {
    if (instance == nullptr) {
        instance = new SerialManager();
    }
    return instance;
}

// SerialManager构造函数
SerialManager::SerialManager() {
    //  TODO 配置波特率
    // 使用Arduino Serial库的begin方法初始化串口通信.初始化串口通信，接受一个波特率参数。
    Serial.begin(ProjectConfig::BAUD);
    // 使用Arduino Serial库的begin方法初始化串口通信.初始化串口通信，接受一个波特率参数。
    Serial1.begin(ProjectConfig::WIFI_BAUD);
};

// println方法定义，打印一条消息并换行
void SerialManager::println(const String &message) {
    Serial.println(message); // 调用Serial.println打印字符串和换行符
}

// available方法定义，检查串口是否有数据可读
bool SerialManager::available() {
    return Serial.available() > 0; // 调用Serial.available检查数据
}

// readString方法定义，读取串口数据直到遇到换行符
String SerialManager::readString() {
    return Serial.readStringUntil('\n'); // 调用Serial.readStringUntil读取数据
}
