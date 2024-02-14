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
    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅NETWORK_STATUS_CHANGE消息
    eventManager->subscribe(NETWORK_STATUS_CHANGE, this);
}


/** 采集温湿度并且显示在LCD屏幕上
 * @param enabled 是否启用该功能
 */
void HardwareAbstraction::processTemperatureAndHumidity(boolean enabled) {
    if (!enabled) return;

    float humidity = sensorManager->readHumidity();
    float temperature = sensorManager->readTemperature();

    displayManager->displayHumidity(humidity);
    displayManager->displayTemperature(temperature);
}


/**
    * 实现Subscriber接口要求的update方法。
    * 更新网络连接状态到LCD屏幕上。
    * @param message 收到的消息（收到它的子类的消息，int类型号）
    */
void HardwareAbstraction::update(const Message &message) {
    DataManager *dataManager = DataManager::getInstance();
    // 需要将Message对象转换为具体类型，消息类型
    const auto &networkMessage = static_cast<const NetworkStatusMessage &>(message);
    ConnectionStatus status = networkMessage.getStatus();
    String data;
    switch (status) {
        case ConnectionStatus::NotConnected:
            data = "Not Connected";
            break;
        case ConnectionStatus::ConnectingToWiFi:
            data = "Connecting WiFi";
            break;
        case ConnectionStatus::WiFiConnected:
            data = "WiFi Connected";
            break;
        case ConnectionStatus::ServerConnected:
            data = "Server Connected";
            break;
        default:
            data = "Unknown Status";
    }

    // 在屏幕下方区域显示网络连接状态
    displayManager->displayBelow(data);
    dataManager->saveData(data, false);
}

// 其他管理器的接口方法实现，如传感器数据读取和执行器控制



