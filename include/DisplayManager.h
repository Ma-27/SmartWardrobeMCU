//
// Created by Mamh on 2024/2/2.
//

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "LiquidCrystal_PCF8574.h"

/**
 * 显示管理器负责管理与显示相关的所有操作，如LED屏幕显示。但不负责管理其他诸如亮灯，串口等等功能。
 * 显示管理器为上层提供显示字符相关的接口。但隐藏了具体硬件实现信息。
 */
class DisplayManager {
private:
    void LCD1602Init();

    static DisplayManager* instance; // 单例实例
    LiquidCrystal_PCF8574 lcd;
    uint8_t address;

    // 私有构造函数
    DisplayManager(uint8_t address);
    // 私有复制构造函数和赋值操作符以防止复制
    DisplayManager(const DisplayManager&);
    DisplayManager& operator=(const DisplayManager&);

public:
    // 获取单例对象的静态方法
    static DisplayManager* getInstance();

    // 显示温湿度的接口
    void displayHumidity(float humidity);
    void displayTemperature(float temperature);
};

#endif


