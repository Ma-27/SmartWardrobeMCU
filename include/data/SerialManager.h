/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/4 上午 08:56
 */

// SerialManager.h
#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h> // 包含Arduino基础头文件，提供Serial等类的定义
#include "utility/ProjectConfig.h"
#include "hardware_abstraction/actuators/ActuatorManager.h"
#include "data/pub-sub/Subscriber.h"

/** 串口管理器  SerialManager类
 * 负责管理，格式化串口有关的打印输出。
 */


class SerialManager : public Subscriber {
private:
    static SerialManager *instance; // 该类的静态实例，采用单例模式
    SerialManager(); // 构造函数声明

    // 用于存储收集到的命令字符串
    String command;

    // 根据接收到的命令执行相应操作
    void executeCommand(String command);

    // 保存事件接收器的一个对象，为了订阅并且接收网络更新的信息。
    EventManager *eventManager;

    /**
    * 实现Subscriber接口要求的update方法。
    * 接受初始化完成的消息，将接受串口信息的任务添加给任务调度器。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;


    // 调用这个方法来处理接收到的串口命令
    void listenCommand(bool enable);

    void initSerialManager();


public:
    SerialManager(const SerialManager &) = delete; // 删除公开的构造方法
    SerialManager &operator=(const SerialManager &) = delete;

    static SerialManager *getInstance(); // Method to get the instance of the class


    // 可变参数模板函数，用于打印多种类型的数据
    template<typename T, typename... Args>
    void print(T first, Args... args) {
        Serial.print(first); // 打印第一个参数
        if (sizeof...(args)) { // 如果还有更多参数...
            Serial.print(", "); // 在参数之间打印逗号和空格作为分隔符
            print(args...); // 递归调用，打印剩余参数
        }
    }

    // 递归终止函数，当参数包为空时调用
    void print() {
        // 不执行任何操作，作为递归的终点
    }

    void println(const String &message); // 打印字符串并添加换行符
    bool available(); // 检查串口是否有数据可读
    String readString(); // 读取串口数据直到遇到换行符
};

#endif // SERIAL_MANAGER_H
