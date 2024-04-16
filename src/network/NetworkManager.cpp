/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 10:15
 */

#include <HardwareSerial.h>
#include <Arduino.h>
#include <avr/dtostrf.h>
#include <ArduinoJson.h>

#include "network/NetworkManager.h"
#include "data/DataManager.h"
#include "utility/ProjectConfig.h"
#include "core/TaskScheduler.h"
#include "network/net_message/PacketGenerator.h"

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
 * 将温湿度等数据实时传送到服务器。
 * 有新消息时执行特定任务
 * @param message 收到的消息
 * @param messageType 收到的消息类型，int类型号
 */
void NetworkManager::update(const Message &message, int messageType) {
    dataManager->logData("init task scheduler ready from network manager", false);

    switch (messageType) {
        case TASK_SCHEDULER_READY:
            // 负责上传温湿度数据到云平台，此过程由NetworkManager层进行。
            TaskScheduler::getInstance().addTask([this]() { this->uploadDhtDataToPlatform(true); },
                                                 ProjectConfig::UPLOAD_DATA_TIME);
            // 延时2s来避免碰撞
            delay(2000);
            // 负责上传光照数据到云平台，此过程由NetworkManager层进行。
            TaskScheduler::getInstance().addTask([this]() { this->uploadLightToPlatform(true); },
                                                 ProjectConfig::UPLOAD_DATA_TIME);
            break;
        default:
            // DO NOTHING
            dataManager->logData("init message error(task scheduler)", true);
    }

}


// 解析命令
bool NetworkManager::parseCommand(const String &command) {
    // 移除字符串首尾的空白字符
    String trimmedCommand = command;
    trimmedCommand.trim();

    if (trimmedCommand.length() > 0) {
        // 如果命令不为空（即含有子层级的命令），递交给其子类处理
        return dispatchCommand(trimmedCommand, "", this);
    } else {
        // 如果命令为空，返回false
        return false;
    }
}


// 具体解析是哪个负责执行命令，派发给相应的监听器
bool NetworkManager::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 删除命令前的所有空格
    command.trim();

    // 执行有-的子命令并且处理参数
    if (command.startsWith("-")) {
        // 去掉命令开头的"-"，以保留"net "的格式
        String processedCommand = command.substring(1);

        if (processedCommand.startsWith("ping")) {
            // 构建信息
            String message = PacketGenerator::ping();
            // 发送ping消息
            networkDataHandler->sendData(message);

            dataManager->logData("Ping Datagram Sent", true);
            // 接收pong消息,由PacketParser处理
            ///FINISH
        }else if (processedCommand.startsWith("dht")) {
                // 构建信息
                String message = PacketGenerator::generateTemperatureHumidityMessage();
                // 发送温湿度数据消息
                networkDataHandler->sendData(message);

                dataManager->logData("Uploading temperature humidity Command executed", true);
                ///FINISH
        }else if (processedCommand.startsWith("light")) {
            // 构建信息
            String message = PacketGenerator::generateLightMessage();
            // 发送光照、电机等等消息
            networkDataHandler->sendData(message);

            dataManager->logData("Uploading light data Command executed", true);
            ///FINISH

            /// 上传云平台裸数据命令
        } else if (processedCommand.startsWith("uploadRaw")) {
            // 假设正确的字符串processedCommand目前是"upload "data""

            // 找到第一个双引号，然后加1跳过它
            int start = processedCommand.indexOf('"') + 1;

            // 如果没有找到双引号，indexOf 返回-1，加1后变为0
            if (start == 0) {
                // 没有找到双引号，返回空字符串
                return "";
            }

            // 从start开始找下一个双引号
            int end = processedCommand.indexOf('"', start);
            if (end == -1) {
                // 如果没有找到第二个双引号，返回空字符串
                return "";
            }

            // 找到要上传的内容，提取并返回两个双引号之间的内容
            String data = processedCommand.substring(start, end);

            networkDataHandler->sendData(data);

        } else {
            // 未知命令
            dataManager->logData("Invalid command parameter in Network Manager: " + processedCommand, true);
            return false;
        }
    }

    // 不继续向下处理
    return true;
}






//-----------------------------------------------------------------------------------------------------------------------------------


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

// 测试MQTT连接
bool NetworkManager::testMQTT() {
    return serverConnector->testMQTT();
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

// 上传温湿度数据到云平台
bool NetworkManager::uploadDhtDataToPlatform(bool enable) {
    if (!enable) return false;

    // 获取数据并且编码成JSON格式
    String data = PacketGenerator::generateTemperatureHumidityMessage();
    // 发送数据
    return networkDataHandler->sendData(data);
}

// 上传光照数据到云平台
bool NetworkManager::uploadLightToPlatform(bool enable) {
    if (!enable) return false;

    // 获取数据并且编码成JSON格式
    String data = PacketGenerator::generateLightMessage();
    // 发送数据
    return networkDataHandler->sendData(data);
}


