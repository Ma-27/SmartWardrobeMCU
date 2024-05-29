/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/6 下午 07:36
 */

#include "network/net_message/PacketQueue.h"
#include "data/DataManager.h"

// 初始化静态实例指针
PacketQueue *PacketQueue::instance = nullptr;

// 提供一个公共的访问方法
PacketQueue *PacketQueue::getInstance() {
    if (instance == nullptr) {
        instance = new PacketQueue();
        instance->initPacketQueue();
    }
    return instance;
}

// 这是该类的默认构造函数
PacketQueue::PacketQueue() {
    // KEEP IT EMPTY
}

// 初始化数据包队列
void PacketQueue::initPacketQueue() {
    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅调度器准备好了的消息。准备好了之后就添加处理数据的任务一直调度运行。
    eventManager->subscribe(TASK_SCHEDULER_READY, this);
}

// 析构函数
PacketQueue::~PacketQueue() {
    // 清理资源
}

// 接收报文并加入队列
void PacketQueue::enqueuePacket(const String &packet) {
    // 将报文加入队列
    packetQueue.push(packet);
}

// 处理队列中的报文
void PacketQueue::processPackets(bool enabled) {
    if (!enabled || packetQueue.empty()) {
        return;
    }

    // 当队列不为空时持续处理
    while (!packetQueue.empty()) {
        // 获取队列前的报文
        String packet = packetQueue.front();

        // 从队列中移除这个报文
        packetQueue.pop();

        // 调用PacketParser解析报文
        parser.parsePacket(packet);
    }
}

/**
    * 实现Subscriber接口要求的update方法。
    * 处理网络发送来的信息的线程。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
void PacketQueue::update(const Message &message, int messageType) {
    switch (messageType) {
        case TASK_SCHEDULER_READY:
            // 初始化PacketParser,为了避免占用大量资源，选择在网络连接后初始化
            parser.initialize();

            // 负责时刻解析报文。
            TaskScheduler::getInstance().addTask([this]() { this->processPackets(true); },
                                                 ProjectConfig::NO_INTERVAL, "parse packets");


            break;
        case NETWORK_STATUS_CHANGE:
            break;
        default:
            // DO NOTHING
            break;
    }
}
