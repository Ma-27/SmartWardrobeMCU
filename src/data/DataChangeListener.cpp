/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/18 上午 08:44
 */

// DataChangeListener.cpp
#include "data/DataChangeListener.h"

DataChangeListener *DataChangeListener::instance = nullptr;

DataChangeListener::DataChangeListener() {
    // 获取DataManager的实例
    dataManager = DataManager::getInstance();
    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
}

DataChangeListener *DataChangeListener::getInstance() {
    // 局部静态变量用于存储单例实例
    if (instance == nullptr) {
        instance = new DataChangeListener();
    }
    return instance;
}

void DataChangeListener::subscribeDataChange() {
    // 订阅NETWORK_STATUS_CHANGE消息
    eventManager->subscribe(NETWORK_STATUS_CHANGE, this);
}

void DataChangeListener::update(const Message &message, int messageType) {
    // 根据messageType处理不同的数据更新
    if (messageType == NETWORK_STATUS_CHANGE) {
        // 示例：更新连接状态。需要将Message对象转换为具体类型，消息类型
        const auto &networkMessage = static_cast<const NetworkStatusMessage &>(message);
        dataManager->connectionStatus = networkMessage.getStatus();
    }
    // 可以根据需要添加更多的消息类型处理
}


