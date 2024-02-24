/**
 * @description: ServerConnector类负责处理网络连接和与服务器的通信。
 * 该类抽象了连接网络和服务器的细节，执行握手，管理连接状态。
 * 它通过卸载直接的网络和服务器交互任务，简化了NetworkManager的职责。
 * 使用单例模式确保全局只有一个ServerConnector实例。
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/21 下午 08:56
 */

#ifndef SERVER_CONNECTOR_H
#define SERVER_CONNECTOR_H

#include "Arduino.h"
#include "data/DataManager.h"
#include "hardware_abstraction/display/DisplayManager.h"

class DataManager; // 前向声明
class DisplayManager;


class ServerConnector {
private:
    // 单例实例
    static ServerConnector *instance;

    // 数据管理器实例
    DataManager *dataManager;

    DisplayManager *displayManager;

    // 私有构造函数，遵循单例模式
    ServerConnector();

    // 执行AT测试和AT命令连接
    bool executeAT_Setting(const char *data, const char *keyword, unsigned long time_out);

public:
    // 禁止拷贝构造和赋值操作
    ServerConnector(const ServerConnector &) = delete;

    ServerConnector &operator=(const ServerConnector &) = delete;

    // 获取单例实例的方法
    static ServerConnector *getInstance();

    // 连接到服务器
    bool configWifiSettings();

    // 获得服务器的握手信息，如果显示连接上了，那就ok了
    bool readServerShakehands();

    // 从服务器断开连接
    bool disconnectFromServer();

    // 测试AT+RST 复位连接
    bool resetConnection();

};

#endif // SERVER_CONNECTOR_H