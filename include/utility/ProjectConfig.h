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
    // 设备ID编号，用于云平台识别设备和其他操作
    static const int DEVICE_ID = 1;

    // 服务器ID编号，用于云平台识别服务器和其他操作
    static const int SERVER_ID = 0;

    // 更新温湿度数据的频率,单位为ms
    static const int UPDATE_DHT_TIME = 100;

    /// 控制温湿度数据的频率,单位为ms。注意，这个时间是可以动态修改的。采用PID算法，在算法中会改动
    static const int CONTROL_DHT_TIME = 250;

    /// 更新各种指标的频率为立即更新，没有延迟,单位为ms
    static const int NO_INTERVAL = 0;

    /// 更新各种指标的频率为小的延迟,单位为ms
    static const int SMALL_INTERVAL = 20;

    /// 更新各种指标的频率为中等的延迟,单位为ms
    static const int MEDIUM_INTERVAL = 50;

    /// 更新各种指标的频率为大的延迟,单位为ms
    static const int BIG_INTERVAL = 200;

    // 串口和单片机的波特率设为115200.
    static const long BAUD = 115200;

    // 单片机和ESP01通信的波特率设为115200.
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
#define WIFI_SSID "306-18G"
#define WIFI_PASSWORD "cqupt306?"
// #define WIFI_SSID "mamhPhone"
// #define WIFI_PASSWORD "12345678"

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

    // 用于存储从服务器接收到的数据，(设置为1.5KB。因为服务器返回的数据最多为1KB)
    static const int BUFFER_SIZE = 1536; // 1.5KB

    // 湿度控制器(增加湿度）的线使用Arduino的4号引脚
    static const int HUMID_INCREASE_CONTROL_PIN = 4;

    // 湿度控制器(降低湿度）的线使用Arduino的5号引脚
    static const int HUMID_DECREASE_CONTROL_PIN = 5;

    // 温度控制器(降温风扇）的线使用Arduino的6号引脚
    static const int COOL_CONTROL_PIN = 6;

    // 温度控制器(增温热发生器）的线使用Arduino的7号引脚
    static const int HEAT_CONTROL_PIN = 7;

    // 舵机控制引脚
    static const int SERVO_PIN = 13;

    // 步进电机控制引脚1
    static const int MOTOR_IN1 = 24;

    // 步进电机控制引脚2
    static const int MOTOR_IN2 = 25;

    // 步进电机控制引脚3
    static const int MOTOR_IN3 = 26;

    // 步进电机控制引脚4
    static const int MOTOR_IN4 = 27;

    // 衣架的数量，根据实物，1cm的位置对应1个衣架。5个衣架位置和1个默认位置
    static const int SHELF_NUMS = 6;
};

#endif // PROJECT_CONFIG_H

