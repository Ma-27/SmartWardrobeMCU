/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/22 下午 08:34
 */

#ifndef NETWORKDATAHANDLER_H
#define NETWORKDATAHANDLER_H

#include <WString.h>
#include "network/net_message/NetworkPacketType.h"
#include "data/pub-sub/Subscriber.h"
#include "network/net_message/PacketQueue.h"

class DataManager; // 前向声明
class EventManager; // 前向声明



class NetworkDataHandler : public Subscriber {
public:
    // 禁止拷贝构造和赋值操作
    NetworkDataHandler(const NetworkDataHandler &) = delete;

    NetworkDataHandler &operator=(const NetworkDataHandler &) = delete;

    // 获取单例对象的接口
    static NetworkDataHandler *getInstance();

    // 发送数据到服务器
    bool sendData(const String &data);

    // 从服务器接收数据
    String receiveData(bool enabled);

    // 验证数据消息。给定报文类型MessageType，验证报文内容是否符合预期，或者读取报文状态
    bool validatePacket(String packet, int packetType);

private:
    // 私有化构造函数和析构函数，确保通过getInstance()管理实例
    NetworkDataHandler();

    ~NetworkDataHandler() override;

    // 初始化网络数据处理器,这才是真正的构造函数
    void initNetworkDataHandler();

    // 单例实例
    static NetworkDataHandler *instance;

    // 数据管理器实例，用于数据处理
    DataManager *dataManager;

    // 事件管理器实例，用于注册需要监听的事件
    EventManager *eventManager;

    // 数据包队列实例，用于存储接收到的数据包并且处理
    PacketQueue *packetQueue;

    /**
    * 实现Subscriber接口要求的update方法。
    * 将随时监听服务器发来的数据。
    * @param message 事件消息
    * @param messageType 事件消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;
};

#endif // NETWORKDATAHANDLER_H
