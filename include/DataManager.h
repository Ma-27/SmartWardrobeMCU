//
// Created by Mamh on 2024/2/3.
//

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <WString.h>

/**
 * **数据管理器** (`DataManager`)
   - 中心化管理和调度数据流。负责数据的存储和校验，协调数据的在各个组件之间的流通等等。
   - 结合串口管理器 (SerialManager)，利用串口通信进行调试信息的输出和读取。
 */
class DataManager {
private:
    static DataManager *instance;

    // FIXME
    // 假设存储的是这个数据
    String storedData;

    DataManager(); // 私有构造函数

public:
    DataManager(const DataManager &) = delete; // 外部不可见默认的构造方法
    DataManager &operator=(const DataManager &) = delete;

    static DataManager *getInstance(); // 获取单例对象的方法
    void storeData(const String &data); // 存储数据的方法
    void sendData(const String &data, bool SerialPrint); // 发送数据的方法
};

#endif


