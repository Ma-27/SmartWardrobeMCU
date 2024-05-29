/**
 * @description: 加热控制类，用于控制加热模块的开关。使用数字 IO，高电平工作，低电平不工作。
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/30 上午 10:34
 */

#include "hardware_abstraction/actuators/Heater.h"
#include "data/DataManager.h"

// 初始化静态实例指针，确保全局仅有一个加热器实例
Heater *Heater::instance = nullptr;

// 提供一个全局访问点，获取加热器实例
Heater *Heater::getInstance() {
    if (instance == nullptr) {
        // 如果实例未创建，则新建一个。直接在构造方法中会遇到创建不完整的问题
        instance = new Heater();
        // 初始化加热器
        instance->initHeater();
    }
    return instance;
}

// 私有的构造函数，支持单例模式
Heater::Heater() {
    // 构造函数保持为空，实际的初始化操作在initHeater中完成
}

// 初始化加热器的配置，如设置控制引脚
void Heater::initHeater() {
    // 设置加热器的控制引脚为输出模式
    pinMode(ProjectConfig::HEAT_CONTROL_PIN, OUTPUT);
}

// 控制加热器开启，设置相应的控制引脚为高电平
void Heater::turnOn() {
    digitalWrite(ProjectConfig::HEAT_CONTROL_PIN, HIGH);
    // 更新加热器状态为开启
    isOn = true;
}

// 控制加热器关闭，设置相应的控制引脚为低电平
void Heater::turnOff() {
    digitalWrite(ProjectConfig::HEAT_CONTROL_PIN, LOW);
    // 更新加热器状态为关闭
    isOn = false;
}

// 解析传入的控制命令，判断是否有效并执行相应操作
bool Heater::parseCommand(const String &command) {
    // 去除命令字符串的首尾空格
    String trimmedCommand = command;
    trimmedCommand.trim();

    // 检查命令长度，非空则处理
    if (trimmedCommand.length() > 0) {
        return dispatchCommand(trimmedCommand, "", this);
    } else {
        return false;
    }
}

// 处理具体的命令，根据命令内容执行开或关操作
bool Heater::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();

    // 再次确保命令无额外空格
    command.trim();
    // 去除命令的首字符
    String processedCommand = command.substring(1);
    // 再次去除处理后命令的首尾空格
    processedCommand.trim();

    // 根据命令内容执行相应操作

    // 执行开启加热器
    if (processedCommand.startsWith("on")) {
        turnOn();
    }
        // 执行关闭加热器
    else if (processedCommand.startsWith("off")) {
        turnOff();
    }
        // 未知命令，打印错误信息
    else {
        dataManager->logData("Unknown command in Heater: " + processedCommand, true);
        return false;
    }
    return true;
}

