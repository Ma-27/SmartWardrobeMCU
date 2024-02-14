//
// Created by Mamh on 2024/2/6.
//

/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/6 上午 08:08
 */

// EventManager.h TODO 使用多态基类作为消息的方法来实现发布-订阅模式
#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

// 前向声明Subscriber类
class Subscriber; // 前向声明，代替#include "data/pub-sub/Subscriber.h"

#include <ArduinoSTL.h>
#include <Arduino.h>
#include <map>
#include <vector>
#include <algorithm>

#include "data/pub-sub/Message.h" // 包含消息类型定义
#include "data/DataManager.h"

class EventManager {
private:
    // 事件类型到订阅者列表的映射
    std::map<int, std::vector<Subscriber *>> subscribersMap;

    static EventManager *instance; // 静态实例指针

    EventManager() {} // 私有构造函数
    EventManager(const EventManager &) = delete; // 禁止复制
    EventManager &operator=(const EventManager &) = delete; // 禁止赋值

    // @deprecated
    // DataManager *dataManager;


public:
    // 获取实例的方法
    static EventManager *getInstance();

    // 订阅方法，允许订阅者订阅多个消息类型
    void subscribe(int messageType, Subscriber *subscriber);

    // 取消订阅方法
    void unsubscribe(int messageType, Subscriber *subscriber);

    // 通知方法，根据消息类型通知对应的订阅者集合。定义一个通用的notify方法(使用了多态性）
    void notify(int messageType, const Message &message);
};

#endif

