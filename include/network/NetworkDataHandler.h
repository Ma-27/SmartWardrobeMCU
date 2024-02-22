/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/22 下午 08:34
 */

#ifndef NETWORKDATAHANDLER_H
#define NETWORKDATAHANDLER_H

#include <WString.h>

class DataManager; // 前向声明

class NetworkDataHandler {
public:
    // 禁止拷贝构造和赋值操作
    NetworkDataHandler(const NetworkDataHandler &) = delete;

    NetworkDataHandler &operator=(const NetworkDataHandler &) = delete;

    // 获取单例对象的接口
    static NetworkDataHandler *getInstance();

    // 发送数据到服务器
    bool sendData(const String &data);

    // 从服务器接收数据
    String receiveData();

private:
    // 私有化构造函数和析构函数，确保通过getInstance()管理实例
    NetworkDataHandler();

    ~NetworkDataHandler();

    // 单例实例
    static NetworkDataHandler *instance;

    // 数据管理器实例，用于数据处理
    DataManager *dataManager;
};

#endif // NETWORKDATAHANDLER_H
