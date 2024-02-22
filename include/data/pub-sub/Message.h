/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/6 上午 08:10
 */

// Message.h
#ifndef MESSAGE_H
#define MESSAGE_H

// 可以在这里添加更多的事件类型
enum MessageType {
    // 网络状态发生变化（连接/断开/准备）
    NETWORK_STATUS_CHANGE = 0,

    // 任务调度器准备就绪，可以添加循环执行的任务
    TASK_SCHEDULER_READY = 1,

};

class Message {
public:
    virtual ~Message() {

    }
    // 可以添加一些通用的虚函数，供派生类实现
};

#endif

