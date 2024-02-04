//
// Created by Mamh on 2024/2/2.
//

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
    }
    return instance;
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
    lcd.autoscroll();
}

// 显示屏幕下方一行
void DisplayManager::displayBelow(String info) {
    lcd.blink();
    lcd.setCursor(0, 1);
    lcd.print(info);
    lcd.autoscroll();
}


