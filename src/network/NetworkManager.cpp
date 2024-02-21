/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 10:15
 */

#include <HardwareSerial.h>
#include <Arduino.h>
#include "network/NetworkManager.h"
#include "data/DataManager.h"
#include "utility/ProjectConfig.h"

// 初始化静态实例指针
NetworkManager *NetworkManager::instance = nullptr;

// 提供一个公共的访问方法
NetworkManager *NetworkManager::getInstance() {
    if (instance == nullptr) {
        instance = new NetworkManager();
    }
    return instance;
}

// 初始化所有网络相关的组件，这是该类的默认构造函数
NetworkManager::NetworkManager() {
    // 获得DataManager实例
    dataManager = DataManager::getInstance();

    // 获得EventManager实例
    eventManager = EventManager::getInstance();

    // 获得ServerConnector实例
    serverConnector = ServerConnector::getInstance();
}



/**
 * 连接到指定的Wi-Fi接入点AP，接入互联网。同时配置各种网络设置。
 */
bool NetworkManager::connectToWifi() {
    bool result = true;
    // 如果其中一个失败了，那就返回失败。

    // 提示正在连接的信息
    dataManager->saveData("CONNECTING TO WIFI", true);
    setConnectionStatus(ConnectionStatus::ConnectingToWiFi);

    // 连接到Wi-Fi网络
    result = serverConnector->configWifiSettings() && result;

    // 提示成功连上了AP的信息
    if (result) {
        dataManager->saveData("CONNECT SUCCEEDED TO WIFI", true);
        setConnectionStatus(ConnectionStatus::WiFiConnected);
    }

    // 检验服务器发回的信息正确性
    result = readServerShakehands() && result;

    // 提示成功连上了iot云端平台的信息
    if (result) {
        dataManager->saveData("SERVER CONNECTED", true);
        setConnectionStatus(ConnectionStatus::ServerConnected);
    }

    return result;
}

/**
 * 获得服务器的握手信息，如果显示连接上了，那就ok了
 * @return 返回是否成功
 */
bool NetworkManager::readServerShakehands() {
    return serverConnector->readServerShakehands();
}

// 断开和当前路由器的连接
bool NetworkManager::disconnect() {
    return serverConnector->disconnectFromServer();
}

// 测试AT+RST 复位连接
bool NetworkManager::resetConnection() {
    return serverConnector->resetConnection();
}


// 添加用于获取和设置当前网络状态的方法
ConnectionStatus NetworkManager::getCurrentStatus() {
    return dataManager->connectionStatus;
}

/**设置当前的连接状态。并且发布网络状态更新事件
 * @param status 待设置的网络状态
 */
void NetworkManager::setConnectionStatus(ConnectionStatus status) {
    dataManager->connectionStatus = status;

    // 发布网络状态更新消息
    NetworkStatusMessage message(status);
    Serial.flush();
    // delay(500);
    eventManager->notify(NETWORK_STATUS_CHANGE, message);
    // delay(500);
    Serial.flush();
}

// 上传数据到云平台
bool NetworkManager::uploadDataToPlatform(String data) {
    Serial1.println(data);
    // 保存打印数据到本地
    dataManager->saveData(data, true);
    return true;
}

