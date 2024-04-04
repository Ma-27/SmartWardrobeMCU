/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/6 下午 07:36
 */

#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

#include <queue>
#include <WString.h>
#include "network/net_message/PacketParser.h"
#include "data/pub-sub/Subscriber.h"
// #include "data/pub-sub/EventManager.h"

class EventManager; // 前向声明



class PacketQueue : public Subscriber {
public:
    /// 单例模式
    // 禁止拷贝构造和赋值操作
    PacketQueue(const PacketQueue &) = delete;

    PacketQueue &operator=(const PacketQueue &) = delete;

    // 提供一个公共的访问方法
    static PacketQueue *getInstance();

    // 接收报文并入队列
    void enqueuePacket(const String &packet);

private:
    static PacketQueue *instance; // 静态私有实例指针

    // 私有化构造函数
    PacketQueue();

    // 析构函数
    ~PacketQueue();

    // 用于解析报文
    PacketParser parser;

    // 初始化报文队列
    void initPacketQueue();

    // 存储报文的队列
    std::queue<String> packetQueue;

    // 处理队列中的所有报文
    void processPackets(bool enabled);

    // 保存事件接收器的一个对象，为了订阅并且接收网络连接成功的信息。
    EventManager *eventManager;


    /**
    * 实现Subscriber接口要求的update方法。
    * 处理网络发送来的信息的线程。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;
};

#endif // PACKETQUEUE_H

