//
// Created by Mamh on 2024/2/2.
//

#include "DisplayManager.h"
#include "LiquidCrystal_PCF8574.h"
#include "ProjectConfig.h"

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
    lcd.setBacklight(255);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Closet");
    lcd.setCursor(0, 1);
    lcd.print("hello user");
}

void DisplayManager::displayHumidity(float humidity) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity:");
    lcd.setCursor(9, 0);
    lcd.print(humidity);
    lcd.setCursor(15, 0);
    lcd.print("%");
}

void DisplayManager::displayTemperature(float temperature) {
    lcd.setCursor(0, 1);
    lcd.print("Temperat:");
    lcd.setCursor(9, 1);
    lcd.print(temperature);
    lcd.setCursor(14, 1);
    lcd.print((char) 223); // 摄氏度符号
    lcd.setCursor(15, 1);
    lcd.print("C");
}


