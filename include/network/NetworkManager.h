/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 10:15
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "network/net_message/NetworkStatusMessage.h"
#include "network/ServerConnector.h"
#include "data/pub-sub/Subscriber.h"
#include "data/pub-sub/EventManager.h"
#include "NetworkDataHandler.h"
#include "core/CommandListener.h"

class DataManager; // 前向声明
class EventManager; // EventManager也在这里使用，前向声明
class ServerConnector; // ServerConnector也在这里使用，前向声明
class NetworkDataHandler; // NetworkDataHandler也在这里使用，前向声明
class DisplayManager; // DisplayManager也在这里使用，前向声明

/**
 * 网络抽象层，负责提供网络和设备的接口，网络的连接和释放，网络状态的监测，网络的数据收发处理等等。
 */
class NetworkManager : public Subscriber, public CommandListener {
private:
    // 静态公共私有实例指针
    static NetworkManager *instance;

    DataManager *dataManager;
    EventManager *eventManager;
    ServerConnector *serverConnector;
    NetworkDataHandler *networkDataHandler;

    DisplayManager *displayManager;

    // 私有化构造函数
    NetworkManager();

    void initNetworkManager();

    // 上传数据到云平台
    bool uploadDataToPlatform(bool enable);

    // 获得服务器的握手信息，如果显示连接上了，那就ok了
    bool readServerShakehands();

    /**
    * 实现Subscriber接口要求的update方法。
    * 将温湿度等数据实时传送到服务器。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;


public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    NetworkManager(const NetworkManager &) = delete;

    void operator=(const NetworkManager &) = delete;

    // 提供一个公共的访问方法
    static NetworkManager *getInstance();

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;

    // 添加用于获取和设置当前网络状态的方法
    ConnectionStatus getCurrentStatus();

    // 设置当前的连接状态。并且发布网络状态更新事件
    void setConnectionStatus(ConnectionStatus status);

    // 测试AT+RST 复位连接
    bool resetConnection();

    // 断开和当前路由器的连接
    bool disconnect();

    // 连接到指定的Wi-Fi接入点AP，接入互联网
    bool connectToWifi();

    // 测试MQTT指令是否可用
    bool testMQTT();



};

#endif
