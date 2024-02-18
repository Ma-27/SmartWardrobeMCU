//
// Created by Mamh on 2024/2/4.
//

// SerialManager.h
#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h> // 包含Arduino基础头文件，提供Serial等类的定义
#include "utility/ProjectConfig.h"

/** 串口管理器  SerialManager类
 * 负责管理，格式化串口有关的打印输出。
 */


class SerialManager {
private:
    static SerialManager *instance; // 该类的静态实例，采用单例模式
    SerialManager(); // 构造函数声明


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
