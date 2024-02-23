/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 09:30
 */


#include <avr/dtostrf.h>
#include "core/CoreController.h"
#include "core/CoreControllerBuilder.h"
#include "utility/ProjectConfig.h"
#include "data/DataChangeListener.h"
#include "data/pub-sub/ScheduleTaskMessage.h"

CoreController *CoreController::instance = nullptr;

CoreController *CoreController::getInstance() {
    if (instance == nullptr) {
        // 按照builder模式实例化CoreController
        CoreControllerBuilder builder;
        // 创建方法:Manager::getInstance()->检查实例并调用私有构造方法创建。
        instance = builder
                .setDataManager(DataManager::getInstance())
                .setHardwareAbstraction(HardwareAbstraction::getInstance())
                .setNetworkManager(NetworkManager::getInstance())
                .build();
    }
    return instance;
}


/** 核心控制单元中，上电初始化时首先执行的函数。
 * TODO 要严格注意其执行顺序
 */
void CoreController::init() {
    // 为DataManager订阅数据更新。
    DataChangeListener *dataChangeListener = DataChangeListener::getInstance();
    dataChangeListener->subscribeDataChange();


    // 连接网络。
    bool result = connectToWifi();

    if (!result) {
        // 如果连接失败，记录日志
        data->logData("Failed to connect to Wi-Fi", false);
        data->connectionStatus = ConnectionStatus::ERROR;
    }

    // 返回连接结果,1表示成功，0表示失败。
    data->logData(String(result), false);


    // TODO 接下来的是任务调度，模拟线程为每个任务定时执行。当到了规定时间间隔后执行任务。
    // 比如说，使用Lambda表达式安排updateTemperatureAndHumidity作为任务， 更新温湿度
    // 但是各个任务被设计为分布式的初始化，它们注册的话，需要pub-sub模式。Controller通知各个订阅者TASK_SCHEDULER_READY，订阅者就应该将各个任务注册到任务调度器中。

    ScheduleTaskMessage message(EventManagerStatus::AVAILABLE);
    EventManager::getInstance()->notify(TASK_SCHEDULER_READY, message);

    // 分布的任务有：
    // 更新温湿度
    // 上传数据到云平台
}


/**
 * 核心控制单元中，可以一直运行的函数。它负责管理整个循环
 */
void CoreController::looper() {
    // 运行任务调度器
    scheduler.run();
}

// 连接到iot服务器
bool CoreController::connectToWifi() {
    // 连接到指定的Wi-Fi接入点AP，接入互联网
    return network->connectToWifi();
}


