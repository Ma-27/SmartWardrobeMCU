/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/22 下午 08:34
 */

#include "network/NetworkDataHandler.h"
#include "data/DataManager.h"

// 初始化静态成员变量
NetworkDataHandler *NetworkDataHandler::instance = nullptr;

NetworkDataHandler *NetworkDataHandler::getInstance() {
    if (instance == nullptr) {
        instance = new NetworkDataHandler();
    }
    return instance;
}

NetworkDataHandler::NetworkDataHandler() {
    // 获取DataManager的实例
    dataManager = DataManager::getInstance();
}

NetworkDataHandler::~NetworkDataHandler() {
    // 这里可以处理析构时的清理工作
}


bool NetworkDataHandler::sendData(const String &data) {
    // TODO 发送数据（模板化）
    Serial1.println(data);
    // 保存打印数据到本地
    dataManager->logData("Uploading:" + String(data), true);

    // FIXME there always return true
    return true;
}

String NetworkDataHandler::receiveData() {
    // TODO 接收数据
    String data = "";
    return data;
}
