/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 下午 03:11
 */

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <WString.h>
#include "data/SerialManager.h"
#include "network/NetworkManager.h"
#include "data/pub-sub/EventManager.h"

/**
 * **数据管理器** (`DataManager`)
   - 中心化管理和调度数据流。负责数据的存储和校验，协调数据的在各个组件之间的流通等等。
   - 结合串口管理器 (SerialManager)，利用串口通信进行调试信息的输出和读取。
 */
class DataManager {
private:
    // 类自己的对象
    static DataManager *instance;

    DataManager(); // 私有构造函数

    // FIXME 存储数据模式有待优化
    // 假设存储的是这个数据
    String storedData;
    // 串口管理器
    SerialManager *serialManager;

    // pub-sub Event
    EventManager *eventManager;

public:
    // 外部不可见默认的构造方法
    DataManager(const DataManager &) = delete;

    DataManager &operator=(const DataManager &) = delete;

    // 获取单例对象的方法
    static DataManager *getInstance();

    // 发送数据的方法
    void saveData(const String &data, bool SerialPrint);

    // 湿度
    float humidity;

    // 湿度
    float temperature;

    // 光照强度
    int light;

    // 连接信息
    ConnectionStatus connectionStatus = ConnectionStatus::NotConnected; // 初始化为未连接
};

#endif


