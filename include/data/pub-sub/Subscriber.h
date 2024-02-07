//
// Created by Mamh on 2024/2/6.
//

// Subscriber.h
#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "Message.h"

class EventManager; // 前向声明，代替直接包含"EventManager.h"

class Subscriber {
public:
    // 默认构造方法
    virtual ~Subscriber() = default;

    // 使用模板声明一个泛型的update方法
    virtual void update(const Message &message) = 0;
};

#endif

