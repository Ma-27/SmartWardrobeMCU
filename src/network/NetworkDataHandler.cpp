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
        instance->initNetworkDataHandler();
    }
    return instance;
}

void NetworkDataHandler::initNetworkDataHandler() {
    // 获取DataManager的实例
    dataManager = DataManager::getInstance();

    // 获取EventManager的实例
    eventManager = EventManager::getInstance();

    // 获取PacketQueue的实例
    packetQueue = PacketQueue::getInstance();

    // 订阅调度器准备好了的消息。准备好了之后就添加发送数据的任务让调度器一直调度。
    eventManager->subscribe(TASK_SCHEDULER_READY, this);
}

NetworkDataHandler::NetworkDataHandler() {
    // KEEP IT EMPTY
}

NetworkDataHandler::~NetworkDataHandler() {
    // 这里可以处理析构时的清理工作
}


bool NetworkDataHandler::sendData(const String &data) {
    Serial1.println(data);

    // 保存打印数据到本地
    dataManager->logData("=======Uploading To server =======", true);
    dataManager->logData(data, true);
    dataManager->logData("==================================", true);

    // FIXME there always return true
    return true;
}


// FIXME 一直挂起这个线程，接收数据，现在是一直接收不处理
String NetworkDataHandler::receiveData(bool enabled) {
    // 如果没有启用，直接退出
    if (!enabled) {
        return "";
    }

    // 如果根本没收到数据，就直接退出不处理
    if (!Serial1.available()) {
        return "";
    }

    // 获取当前时间
    unsigned long startTime = millis();
    // 设置超时时间，例如1秒
    unsigned long timeout = 2000;

    // 用于存储从服务器接收到的数据，设置为1.25KB。因为服务器返回的数据最多为1KB
    const int bufferSize = 1280; // 1.25KB
    char tempSting[bufferSize] = {0}; // 初始化所有元素为0

    // 当前填充到tempResponse中的位置
    int currentIndex = 0;

    // 检查是否有数据可读，或者是否超时
    while ((millis() - startTime) < timeout && currentIndex < bufferSize - 1) {
        if (Serial1.available()) { // 如果有数据可读
            // 读取一个字符
            char c = Serial1.read();

            // 将字符添加到响应字符串中，确保不会溢出
            tempSting[currentIndex++] = c;
        }
    }

    // 确保字符串正确结束
    tempSting[currentIndex] = '\0';

    // 将字符数组转换为String对象
    String response = String(tempSting);
    response.trim();

    // 如果接收到了响应
    if (response.length() > 0) {
        dataManager->logData("----Receiving data from server ----", true);
        dataManager->logData(response, true);
        dataManager->logData("-----------------------------------", true);

        // 将响应数据添加到数据包队列中
        packetQueue->enqueuePacket(response);

    } else {
        // 超时，没有接收到响应
        dataManager->logData("There is response, but invalid, from server.", true);
    }


    return response;
}


// 验证数据消息。给定报文类型packetType，验证报文内容是否符合预期，或者读取报文状态
bool NetworkDataHandler::validatePacket(String packet, int packetType) {
    if (packetType == NetworkPacketType::CONFIRM) {
        // 用于追踪大括号的计数器
        int braceCount = 0;
        for (unsigned int i = 0; i < packet.length(); ++i) {
            if (packet[i] == '{') {
                // 遇到开括号，计数器增加
                braceCount++;
            } else if (packet[i] == '}') {
                // 遇到闭括号，计数器减少
                braceCount--;
                if (braceCount < 0) {
                    // 如果计数器变为负数，表示有闭括号没有对应的开括号
                    return false;
                }
            }
        }
        // 遍历结束后，如果计数器为零，表示所有括号都正确闭合
        return braceCount == 0;
    }

    // FIXME there always return true
    return true;
}

// 实现Subscriber接口要求的update方法。这里
void NetworkDataHandler::update(const Message &message, int messageType) {
    dataManager->logData("init task scheduler ready from network data handler", false);

    switch (messageType) {
        case TASK_SCHEDULER_READY:
            // 一直挂起这个接收数据的进程。
            TaskScheduler::getInstance().addTask([this]() { this->receiveData(false); },
                                                 ProjectConfig::NO_INTERVAL,"network receiver");
            break;
        default:
            // DO NOTHING
            dataManager->logData("unknown message type in network data handler, subscriber, update message method",
                                 true);
    }

}

