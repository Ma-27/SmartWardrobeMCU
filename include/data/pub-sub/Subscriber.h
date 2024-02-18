/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/4 上午 9:25
 */

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
    virtual void update(const Message &message, int messageType) = 0;
};

#endif

