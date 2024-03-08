/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 10:15
 */

#include <HardwareSerial.h>
#include <Arduino.h>
#include <avr/dtostrf.h>
#include "network/NetworkManager.h"
#include "data/DataManager.h"
#include "utility/ProjectConfig.h"
#include "core/TaskScheduler.h"

// 初始化静态实例指针
NetworkManager *NetworkManager::instance = nullptr;

// 提供一个公共的访问方法
NetworkManager *NetworkManager::getInstance() {
    if (instance == nullptr) {
        instance = new NetworkManager();
        instance->initNetworkManager();
    }
    return instance;
}

// 这是该类的默认构造函数。但是为了防止单例创造不完全问题，必须留空
NetworkManager::NetworkManager() {
    // KEEP IT EMPTY
}

// 初始化所有网络相关的组件，
void NetworkManager::initNetworkManager() {
    // 获得DataManager实例
    dataManager = DataManager::getInstance();

    // 获得EventManager实例
    eventManager = EventManager::getInstance();

    // 获得ServerConnector实例
    serverConnector = ServerConnector::getInstance();

    // 获得NetworkDataHandler实例
    networkDataHandler = NetworkDataHandler::getInstance();

    // 订阅调度器准备好了的消息。准备好了之后就添加发送数据的任务让调度器一直调度。
    eventManager->subscribe(TASK_SCHEDULER_READY, this);

    // 获得DisplayManager实例
    displayManager = DisplayManager::getInstance();
}


/**
 * 实现Subscriber接口要求的update方法。
 * 将温湿度等数据实时传送到服务器。
 * @param message 收到的消息
 * @param messageType 收到的消息类型，int类型号
 */
void NetworkManager::update(const Message &message, int messageType) {
    dataManager->logData("init task scheduler ready from network manager", false);

    switch (messageType) {
        case TASK_SCHEDULER_READY:
            // 负责上传数据到云平台，此过程由NetworkManager层进行。
            TaskScheduler::getInstance().addTask([this]() { this->uploadDataToPlatform(true); },
                                                 ProjectConfig::UPLOAD_DATA_TIME);
            break;
        default:
            // DO NOTHING
            dataManager->logData("init message error(task scheduler)", true);
    }

}


/**
 * 连接到指定的Wi-Fi接入点AP，接入互联网。同时配置各种网络设置。
 */
bool NetworkManager::connectToWifi() {
    bool result = true;
    // 如果其中一个失败了，那就返回失败。

    // 提示正在连接的信息
    dataManager->logData("CONNECTING TO WIFI", true);
    setConnectionStatus(ConnectionStatus::ConnectingToWiFi);

    // 连接到Wi-Fi网络
    result = serverConnector->configWifiSettings() && result;

    // 提示成功连上了AP的信息
    if (result) {
        dataManager->logData("CONNECT SUCCEEDED TO WIFI", true);
        setConnectionStatus(ConnectionStatus::WiFiConnected);
    }

    // 检验服务器发回的信息正确性
    bool tempResult = readServerShakehands();
    result = tempResult && result;

    // 提示进度条
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 提示成功连上了iot云端平台的信息
    if (tempResult) {
        dataManager->logData("SERVER CONNECTED", true);
        setConnectionStatus(ConnectionStatus::ServerConnected);
    } else {
        dataManager->logData("SERVER CONNECTED FAILED", true);
        setConnectionStatus(ConnectionStatus::ERROR);
    }

    return tempResult;
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
bool NetworkManager::uploadDataToPlatform(bool enable) {
    if (!enable) return false;

    char c[100];
    // sprintf 在 Arduino 中无法转换浮点数
    dtostrf(dataManager->temperature, 2, 2, c);
    dtostrf(dataManager->humidity, 2, 2, c + 5);
    // 保存打印数据到本地
    dataManager->logData("Uploading:" + String(c), true);

    // 调用数据收发类收发数据
    return networkDataHandler->sendData(c);
}

