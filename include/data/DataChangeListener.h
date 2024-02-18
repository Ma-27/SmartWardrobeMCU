/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/18 上午 08:44
 */

// DataChangeListener.h
#ifndef DATA_CHANGE_LISTENER_H
#define DATA_CHANGE_LISTENER_H

#include "pub-sub/Subscriber.h"
#include "DataManager.h"
#include "pub-sub/Message.h"

/**
 * @brief 专注于监听数据变化并更新DataManager的单例类
 *
 * DataChangeListener作为Subscriber的具体实现，监听特定类型的数据变化，
 * 如连接状态，并通过DataManager实例更新这些数据。此类使用单例模式设计，
 * 确保全局只有一个监听器实例。
 */
class DataChangeListener : public Subscriber {
public:
    /**
     * 获取DataChangeListener的单例实例
     *
     * @return DataChangeListener& 单例实例的引用
     */
    static DataChangeListener *getInstance();

    /**
     * 删除复制构造函数和赋值操作符，防止复制实例
     */
    DataChangeListener(const DataChangeListener &) = delete;

    DataChangeListener &operator=(const DataChangeListener &) = delete;

    /**
    * 订阅需要DataManager更新的事件集合，以便于更新数据
    *
    */
    void subscribeDataChange();

    /**
     * 处理接收到的消息，根据消息类型更新DataManager中的数据
     *
     * @param message 接收到的消息内容
     * @param messageType 消息的类型
     */
    virtual void update(const Message &message, int messageType) override;

private:
    // 单例实例
    static DataChangeListener *instance;

    /**
     * 私有构造函数，防止外部实例化
     */
    DataChangeListener();

    /**
     * 指向DataManager实例的指针，用于更新数据
     */
    DataManager *dataManager;

    /**
     *  保存事件接收器的一个对象，用于订阅并且接收网络更新的信息。
     */
    EventManager *eventManager;
};

#endif
