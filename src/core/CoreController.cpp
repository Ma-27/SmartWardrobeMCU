//
// Created by Mamh on 2024/2/3.
//


#include "core/CoreController.h"
#include "core/CoreControllerBuilder.h"
#include "utility/ProjectConfig.h"

CoreController *CoreController::instance = nullptr;

CoreController *CoreController::getInstance() {
    if (instance == nullptr) {
        // 按照builder模式实例化CoreController
        CoreControllerBuilder builder;
        // 创建方法:Manager::getInstance()->检查实例并调用私有构造方法创建。
        instance = builder
                .setDataManager(DataManager::getInstance())
                .setHardwareAbstraction(HardwareAbstraction::getInstance())
                .setNetworkManager(NetworkManager::getInstance())
                .build();
    }
    return instance;
}

// 负责更新温湿度，此过程由HardwareAbstraction层进行。此层管理该功能是否启用。
void CoreController::updateTemperatureAndHumidity(int updateFreq) {
    // 延迟规定时间再更新
    delay(updateFreq);

    // 获取到硬件抽象层实例，并且使用硬件抽象层更新温湿度，不经过此层控制
    hardware->processTemperatureAndHumidity(true);
}


//核心控制单元中，上电初始化时首先执行的函数。TODO 要严格注意其执行顺序
void CoreController::init() {
    // 连接网络。
    bool result = connectToWifi();

    // 返回连接结果,1表示成功，0表示失败。
    data->saveData(String(result), false);
}

//核心控制单元中，可以一直运行的函数。它负责管理整个循环
void CoreController::looper() {
    // 更新温湿度
    updateTemperatureAndHumidity(ProjectConfig::LOOPER_UPDATE_TIME);
}

// 连接到iot服务器并且握手
bool CoreController::connectToWifi() {
    bool result = true;
    // 如果其中一个失败了，那就返回失败。

    // 提示正在连接的信息
    data->saveData("CONNECTING TO WIFI", true);
    network->setConnectionStatus(ConnectionStatus::ConnectingToWiFi);

    // 连接到Wi-Fi网络
    result = network->connectWifi() && result;

    // 提示成功连上了AP的信息
    if (result) {
        data->saveData("CONNECT SUCCEEDED TO WIFI", true);
        network->setConnectionStatus(ConnectionStatus::WiFiConnected);
    }

    // 检验服务器发回的信息正确性
    result = network->readServerShakehands() && result;

    // 提示成功连上了iot云端平台的信息
    if (result) {
        data->saveData("SERVER CONNECTED", true);
        network->setConnectionStatus(ConnectionStatus::ServerConnected);
    }

    return result;
}

