/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 下午 8:48
 */

#include "hardware_abstraction/display/DisplayManager.h"
#include "LiquidCrystal_PCF8574.h"
#include "utility/ProjectConfig.h"

// 初始化静态成员变量
DisplayManager* DisplayManager::instance = nullptr;

// 私有构造函数
DisplayManager::DisplayManager(uint8_t addr) : address(addr) {
    LCD1602Init();
}

// 获取单例实例的方法
DisplayManager* DisplayManager::getInstance() {
    if (instance == nullptr) {
        instance = new DisplayManager(ProjectConfig::LCD_ADDRESS);
        instance->initDisplayManager();
    }
    return instance;
}

void DisplayManager::initDisplayManager() {
    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅NETWORK_STATUS_CHANGE消息
    eventManager->subscribe(NETWORK_STATUS_CHANGE, this);
}

/**
    * 实现Subscriber接口要求的update方法。
    * 更新网络连接状态到LCD屏幕上。
    * @param message 收到的消息（收到它的子类的消息，int类型号）
    */
void DisplayManager::update(const Message &message) {
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
    dataManager->saveData(data, false);
}


// 显示进系统欢迎字符，这也是一个测试。
void DisplayManager::LCD1602Init() {
    lcd.begin(16, 2);
    // 背光度，值从0-255变化。这里变为100，防止太亮
    lcd.setBacklight(0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Closet");
    lcd.setCursor(0, 1);
    lcd.print("hello user");
}

// 显示湿度
void DisplayManager::displayHumidity(float humidity) {
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.print("Humidity:");
    lcd.setCursor(9, 0);
    lcd.print(humidity);
    lcd.setCursor(15, 0);
    lcd.print("%");
}

// 显示温度
void DisplayManager::displayTemperature(float temperature) {
    lcd.blink();
    lcd.setCursor(0, 1);
    lcd.print("Temperat:");
    lcd.setCursor(9, 1);
    lcd.print(temperature);
    lcd.setCursor(14, 1);
    lcd.print((char) 223); // 摄氏度符号
    lcd.setCursor(15, 1);
    lcd.print("C");
}

// 显示屏幕上方一行
void DisplayManager::displayUpper(String info) {
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.print(info);
}

// 显示屏幕下方一行
void DisplayManager::displayBelow(String info) {
    lcd.blink();
    lcd.setCursor(0, 1);
    lcd.print(info);
}


