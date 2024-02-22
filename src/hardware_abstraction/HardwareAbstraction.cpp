/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 08:37
 */

#include "hardware_abstraction/HardwareAbstraction.h"
#include "hardware_abstraction/sensors/SensorManager.h"  // 确保包含SensorManager的头文件
#include "utility/ProjectConfig.h"
#include "data/DataManager.h"
#include "core/TaskScheduler.h"
#include "core/CoreController.h"

// 初始化静态实例指针
HardwareAbstraction *HardwareAbstraction::instance = nullptr;

// 提供一个公共的访问方法
HardwareAbstraction *HardwareAbstraction::getInstance() {
    if (instance == nullptr) {
        instance = new HardwareAbstraction();
        instance->initHAL();
    }
    return instance;
}

HardwareAbstraction::HardwareAbstraction() {
    // KEEP IT EMPTY
}

// 初始化所有硬件组件，将原本在构造函数中的初始化逻辑转移到一个单独的init方法中
void HardwareAbstraction::initHAL() {
    // 获取显示组件的实例
    displayManager = DisplayManager::getInstance();
    // 获取传感器组件的实例
    sensorManager = SensorManager::getInstance();
    // 获取数据中心的实例
    dataManager = DataManager::getInstance();

    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅调度器准备好了的消息。准备好了之后就添加任务到调度器一直调度。
    eventManager->subscribe(TASK_SCHEDULER_READY, this);
}


/**
* 实现Subscriber接口要求的update方法。
* 一旦任务调度器准备好了，就将更新温湿度等传感器的任务添加到队列。
* @param message 收到的消息
* @param messageType 收到的消息类型，int类型号
*/
void HardwareAbstraction::update(const Message &message, int messageType) {
    switch (messageType) {
        case TASK_SCHEDULER_READY:
            dataManager->logData("init task scheduler ready from hardware abstraction", false);

            // 负责更新温湿度，此过程由HardwareAbstraction层进行。
            TaskScheduler::getInstance().addTask([this]() { this->processTemperatureAndHumidity(true); },
                                                 ProjectConfig::UPDATE_DHT_TIME);
            break;
        default:
            // DO NOTHING
            dataManager->logData("init message error(task scheduler)", true);
    }
}



/** 采集温湿度并且显示在LCD屏幕上
 * @param enabled 是否启用该功能
 */
void HardwareAbstraction::processTemperatureAndHumidity(boolean enabled) {
    if (!enabled) return;

    // 保存温湿度
    dataManager->humidity = sensorManager->readHumidity();
    dataManager->temperature = sensorManager->readTemperature();
    // 显示温湿度
    displayManager->displayHumidity(dataManager->humidity);
    displayManager->displayTemperature(dataManager->temperature);
}

// 其他管理器的接口方法实现，如传感器数据读取和执行器控制



