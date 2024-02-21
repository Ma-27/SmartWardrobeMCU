/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/21 下午 06:05
 */

#include "hardware_abstraction/display/LCDManager.h"


#include "utility/ProjectConfig.h"

LCDManager *LCDManager::instance = nullptr;

LCDManager::LCDManager(uint8_t addr) : lcd(addr), address(addr) {
    // LCD为16x2
    lcd.begin(16, 2);
    // 设置背光.背光度，值从0-255变化。这里变为0，设为无背光模式
    lcd.setBacklight(0);
}

LCDManager *LCDManager::getInstance() {
    if (instance == nullptr) {
        instance = new LCDManager(ProjectConfig::LCD_ADDRESS);
    }
    return instance;
}

// 初始化LCD显示。显示进系统欢迎字符，这也是一个测试。
void LCDManager::initLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Closet");
    lcd.setCursor(0, 1);
    lcd.print("Hello User");
}

// 显示屏幕上方一行
void LCDManager::displayUpper(const String &info) {
    lcd.setCursor(0, 0);
    lcd.print(info);
}

// 显示屏幕下方一行
void LCDManager::displayBelow(const String &info) {
    lcd.setCursor(0, 1);
    lcd.print(info);
}
