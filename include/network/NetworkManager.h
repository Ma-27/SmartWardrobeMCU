//
// Created by Mamh on 2024/2/2.
//

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "NetworkStatusMessage.h"

class DataManager; // 前向声明
class EventManager; // 如果EventManager也在这里使用，也可以前向声明

/**
 * 网络抽象层，负责提供网络和设备的接口，网络的连接和释放，网络状态的监测，网络的数据收发处理等等。
 */
class NetworkManager {
private:
    static NetworkManager *instance; // 静态公共私有实例指针

    DataManager *dataManager;
    EventManager *eventManager;

    // 私有化构造函数
    NetworkManager();

    // 执行AT测试和AT命令连接
    bool executeAT_Setting(const char *data, const char *keyword, unsigned long time_out);

    ConnectionStatus connectionStatus = ConnectionStatus::NotConnected; // 初始化为未连接

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    NetworkManager(const NetworkManager &) = delete;

    void operator=(const NetworkManager &) = delete;

    // 提供一个公共的访问方法
    static NetworkManager *getInstance();

    // 添加用于获取和设置当前网络状态的方法
    ConnectionStatus getCurrentStatus();

    // 设置当前的连接状态。
    void setConnectionStatus(ConnectionStatus status);

    // 获得服务器的握手信息，如果显示连接上了，那就ok了
    bool readServerShakehands();

    // 测试AT+RST 复位连接
    bool resetConnection();

    // 断开和当前路由器的连接
    bool disconnect();

    // 连接到指定的Wi-Fi接入点AP，接入互联网
    bool connectWifi();
};

#endif
