/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/4 上午 08:56
 */

#include "data/SerialManager.h"
#include "core/TaskScheduler.h"
#include "hardware_abstraction/sensors/Camera.h"

SerialManager *SerialManager::instance = nullptr;

SerialManager *SerialManager::getInstance() {
    if (instance == nullptr) {
        instance = new SerialManager();
        instance->initSerialManager();
    }
    return instance;
}

// SerialManager构造函数
SerialManager::SerialManager() {
    // KEEP IT EMPTY
};

// 真正初始化串口管理器的地方
void SerialManager::initSerialManager() {
    //  TODO 配置波特率
    // 使用Arduino Serial库的begin方法初始化串口通信.初始化串口通信，接受一个波特率参数。
    Serial.begin(ProjectConfig::BAUD);
    // 使用Arduino Serial库的begin方法初始化串口通信.初始化串口通信，接受一个波特率参数。
    Serial1.begin(ProjectConfig::WIFI_BAUD);

    // 初始化命令字符串为空
    command = "";

    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅循环已经准备好了的消息
    eventManager->subscribe(TASK_SCHEDULER_READY, this);
}

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

/**
    * 实现Subscriber接口要求的update方法。
    * 更新网络连接状态到LCD屏幕上。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
void SerialManager::update(const Message &message, int messageType) {
    switch (messageType) {
        case TASK_SCHEDULER_READY:
            // 负责查询串口有无指令。
            TaskScheduler::getInstance().addTask([this]() { this->listenCommand(true); },
                                                 ProjectConfig::QUERY_SERIAL_TIME);
            break;
        default:
            // DO NOTHING
            break;
    }
}

// 调用这个方法来处理接收到的串口命令
void SerialManager::listenCommand(bool enable) {
    if (!enable) return;

    while (Serial.available() > 0) {
        // 读取单个字符
        char receivedChar = Serial.read();
        // 每个命令以换行符结束
        if (receivedChar == '\n') {
            executeCommand(command); // 执行命令
            command = ""; // 重置命令字符串
        } else {
            command += receivedChar; // 将接收到的字符添加到命令字符串
        }
    }
}

// 根据接收到的命令执行相应操作
void SerialManager::executeCommand(String command) {
    // 移除字符串首尾的空白字符
    command.trim();
    // 打印收到的命令
    Serial.println("Received command: " + command);

    // 检查是否包含"light on"子字符串
    if (command.indexOf("light on") != -1) {
        // 执行打开操作
        ActuatorManager::getInstance()->setLightIntensity(100);
    }
        // 检查是否包含"light off"子字符串
    else if (command.indexOf("light off") != -1) {
        // 执行关闭操作
        ActuatorManager::getInstance()->setLightIntensity(0);
    } else if (command.indexOf("camera shot") != -1) {
        // FIXME
        // 摄影并打印到端口
        Camera::getInstance()->captureImage();
    } else {
        // 未知命令
        Serial.println("Unknown command: " + command);
    }
}
