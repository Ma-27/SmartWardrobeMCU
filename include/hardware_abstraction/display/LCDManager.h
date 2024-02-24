/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/21 下午 06:05
 */

#ifndef LCD_MANAGER_H
#define LCD_MANAGER_H

#include "LiquidCrystal_PCF8574.h"

class LCDManager {
private:
    LiquidCrystal_PCF8574 lcd;
    static LCDManager *instance;
    uint8_t address;

    // 私有构造函数
    LCDManager(uint8_t addr);

public:
    // 禁止复制构造函数和赋值操作
    LCDManager(const LCDManager &) = delete;

    LCDManager &operator=(const LCDManager &) = delete;

    // 获取单例实例的方法
    static LCDManager *getInstance();

    // 初始化LCD显示
    void initLCD();

    // 显示信息的方法，上方一行，下方一行
    void displayUpper(const String &info);

    void displayBelow(const String &info);

    // 显示进度条
    void displayProgressBar(int percentage, int position);
};

#endif // LCD_MANAGER_H
