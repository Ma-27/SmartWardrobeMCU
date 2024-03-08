/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 11:13
 */

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <Arduino.h> // 引入Arduino库以支持String类型

/**
 * 项目配置文件，负责定义智能衣柜系统中使用的主要参数设置和引脚信息，以便统一管理和调整。
 */

class ProjectConfig {
public:
    // 更新温湿度数据的频率,单位为ms
    static const int UPDATE_DHT_TIME = 100;

    // 串口和单片机的波特率设为115200.
    static const long BAUD = 115200;

    // 串口和单片机的波特率设为115200.
    static const long WIFI_BAUD = 115200;

    // DHT数据线使用Arduino的2号引脚
    static const int DHTPIN = 2;
    // DHT11类型，可以根据需要为DHT11,22,33
    static const int DHTTYPE = 11;

    // LCD 显示屏的I2C从端装置位址
    static const int LCD_ADDRESS = 0x27;

    // arduino的LED pin。
    static const int ledPin = 13;

    // FIXME
    // 定义WiFi名称和密码为静态成员变量
#define WIFI_SSID "3-1-1202"
#define WIFI_PASSWORD "mamh2015"

    // FIXME
    // 连接到OneNetIot平台的私人秘钥
#define IOT_PLATFORM_PRIVATE_KEY "*627051#KimirrCloset#script*"

    // 更新温湿度数据的频率,单位为ms(30S)
    static const int UPLOAD_DATA_TIME = 30000;

    // 光敏电阻连接的模拟输入引脚
    static const int lightSensorPin = A0;

    // 可变电阻连接的模拟输入引脚
    static const int potentiometerPin = A1;

    // LED连接的数字输出引脚
    static const int lightPin = DAC0;

    // 更新光照强度和控制灯开关的频率,单位为ms
    static const int UPDATE_LIGHT_TIME = 100;

    // 循环命令的频率,单位为ms
    static const int QUERY_SERIAL_TIME = 10;

    // 检查摄像头中断标志位的延迟时间.
    static const long CHECK_CAMERA_INTERRUPT_DELAY = 0;
};

#endif // PROJECT_CONFIG_H

