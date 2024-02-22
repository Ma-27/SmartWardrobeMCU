/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 下午 8:48
 */

#include "hardware_abstraction/display/DisplayManager.h"
#include "LiquidCrystal_PCF8574.h"
#include "utility/ProjectConfig.h"
#include "hardware_abstraction/display/LCDManager.h"

// 初始化静态成员变量
DisplayManager *DisplayManager::instance = nullptr;

// 私有构造函数
DisplayManager::DisplayManager() {
    // 构造函数中初始化LCDManager，这里不改变
    lcdManager = LCDManager::getInstance();
}

// 获取单例实例的方法
DisplayManager *DisplayManager::getInstance() {
    if (instance == nullptr) {
        instance = new DisplayManager();
        instance->initDisplayManager();
    }
    return instance;
}

void DisplayManager::initDisplayManager() {
    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅NETWORK_STATUS_CHANGE消息
    eventManager->subscribe(NETWORK_STATUS_CHANGE, this);

    // 可以在这里调用LCDManager的初始化方法
    lcdManager->initLCD(); // 保留初始化LCD显示的调用
}

/**
    * 实现Subscriber接口要求的update方法。
    * 更新网络连接状态到LCD屏幕上。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
void DisplayManager::update(const Message &message, int messageType) {
    if (messageType != MessageType::NETWORK_STATUS_CHANGE) return;

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
    displayBelow(data);
    dataManager->logData(data, false);
}


// 显示温度，在此层自行组织字符串显示
void DisplayManager::displayTemperature(float temperature) {
    // 摄氏度符号 (char) 223
    String string = "Temperat:" + String(temperature) + (char) 223 + "C";

    lcdManager->displayUpper(string);
}

// 显示湿度，在此层自行组织字符串显示
void DisplayManager::displayHumidity(float humidity) {
    // 摄氏度符号 (char) 223
    String string = "Humidity:" + String(humidity) + " %";

    lcdManager->displayBelow(string);
}

// 显示屏幕上方一行
void DisplayManager::displayUpper(String info) {
    lcdManager->displayUpper(info);
}

// 显示屏幕下方一行
void DisplayManager::displayBelow(String info) {
    lcdManager->displayBelow(info);
}


