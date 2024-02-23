/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 下午 8:48
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "LiquidCrystal_PCF8574.h"
#include "data/pub-sub/Subscriber.h"
#include "data/pub-sub/EventManager.h"
#include "hardware_abstraction/display/LCDManager.h"

class DataManager; // 前向声明

/**
 * 显示管理器负责管理与显示相关的所有操作，如LED屏幕显示。但不负责管理其他诸如亮灯，串口等等功能。
 * 显示管理器为上层提供显示字符相关的接口。但隐藏了具体硬件实现信息。
 */
class DisplayManager : public Subscriber {
private:
    void LCD1602Init();

    static DisplayManager *instance; // 单例实例
    LiquidCrystal_PCF8574 lcd;
    uint8_t address;

    // 私有构造函数
    DisplayManager();

    // 保存事件接收器的一个对象，为了订阅并且接收网络更新的信息。
    EventManager *eventManager;

    // 添加LCDManager引用
    LCDManager *lcdManager;

    // 添加DataManager引用
    DataManager *dataManager;

    /**
    * 实现Subscriber接口要求的update方法。
    * 更新网络连接状态到LCD屏幕上。
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;

    // 初始化显示管理器
    void initDisplayManager();

public:
    // 禁止复制构造函数和赋值操作
    DisplayManager(const DisplayManager &) = delete;

    DisplayManager &operator=(const DisplayManager &) = delete;

    // 获取单例对象的静态方法
    static DisplayManager *getInstance();


    // 显示温湿度的接口
    void displayHumidity(float humidity);
    void displayTemperature(float temperature);

    // 显示进度条
    void displayProgressBar(int value, int position);

    // 在屏幕上方一行显示info
    void displayUpper(String info);

    // 在屏幕下方一行显示info
    void displayBelow(String info);

    void showConnectionStage(const Message &message);
};

#endif


