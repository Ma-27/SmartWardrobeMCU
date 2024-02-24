/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/21 下午 06:05
 */

#include "hardware_abstraction/display/LCDManager.h"


#include "utility/ProjectConfig.h"

// 自定义进度条字符集合
byte fullBlock[8] = {
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111
};

LCDManager *LCDManager::instance = nullptr;

LCDManager::LCDManager(uint8_t addr) : lcd(addr), address(addr) {
    // LCD为16x2
    lcd.begin(16, 2);
    // 设置背光.背光度，值从0-255变化。这里变为0，设为无背光模式
    lcd.setBacklight(0);

    // 创建自定义字符，分配到字符位置0
    lcd.createChar(0, fullBlock);
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


/** 显示进度条
 *
 * @param percentage 百分比例，应该介于0-100之间
 * @param position 显示位置，0为上方，1为下方
 */
void LCDManager::displayProgressBar(int percentage, int position) {
    int progressBlocks = map(percentage, 0, 100, 0, 16); // 将百分比映射到0-16个块

    // 设置光标到指定位置
    if (position == 0) {
        lcd.setCursor(0, 0);
    } else {
        lcd.setCursor(0, 1);
    }

    // 显示进度条
    for (int i = 0; i < progressBlocks; i++) {
        lcd.write(byte(0)); // 显示完全填充的自定义字符
    }

    // 清除进度条之外的部分
    for (int i = progressBlocks; i < 16; i++) {
        lcd.print(" "); // 使用空格来清除
    }
}

