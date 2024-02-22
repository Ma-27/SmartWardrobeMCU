/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 08:08
 */

#include "data/pub-sub/EventManager.h"
#include "data/pub-sub/Subscriber.h"
#include "data/pub-sub/Message.h" // 包含消息类型定义
#include "data/DataManager.h"


// 在EventManager.cpp中初始化静态成员
EventManager *EventManager::instance = nullptr;

/**
 * 获得EventManager的全局唯一实例。
 * @return EventManager的全局唯一实例。
 */
EventManager *EventManager::getInstance() {
    if (instance == nullptr) {
        instance = new EventManager();
    }
    return instance;
}

/** @deprecated

EventManager::EventManager()

EventManager::EventManager() {
    // 初始化DataManager
    dataManager = DataManager::getInstance();
}
 */

void EventManager::subscribe(int messageType, Subscriber *subscriber) {
    subscribersMap[messageType].push_back(subscriber);
}

void EventManager::unsubscribe(int messageType, Subscriber *subscriber) {
    auto &subscribers = subscribersMap[messageType];
    subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), subscriber), subscribers.end());
}

/** 通知方法，根据消息类型通知对应的订阅者集合。
 * 这里需要考虑如何将基类作为消息的message转发给Subscriber。
   因为Subscriber的update方法期望一个特定类型的参数（比如String），这个特定参数是的基类是Message。
 * @param messageType 消息的类型号（代码中可见消息类型）
 * @param message 要发送的消息
 */
void EventManager::notify(int messageType, const Message &message) {
    int i = 0;
    // 准备保存调试信息
    DataManager *dataManager = DataManager::getInstance();
    // 匹配是否有messageType类型的消息，如果有则发送到订阅者
    if (subscribersMap.find(messageType) != subscribersMap.end()) {
        for (auto *subscriber: subscribersMap[messageType]) {
            i++;
            // 将订阅者的地址保存打印
            // dataManager->logData("address of subscriber", false);
            // Serial.println((uintptr_t)subscriber, HEX);
            // Serial.flush(); // 确保消息发送完毕

            // 给订阅者更新数据
            subscriber->update(message, messageType);
        }
        // 将订阅者的数目保存打印
        dataManager->logData("counting looper nums: ", false);
        dataManager->logData(String(i), false);
    }
}


