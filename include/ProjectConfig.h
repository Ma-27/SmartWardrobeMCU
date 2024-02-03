//
// Created by Mamh on 2024/2/3.
//

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

/**
 * 项目配置文件，负责定义智能衣柜系统中使用的主要参数设置和引脚信息，以便统一管理和调整。
 */

class ProjectConfig {
public:
    // 更新数据的频率,单位为ms
    static const int LOOPER_UPDATE_TIME = 500;

    // 串口和单片机的波特率设为9600.
    static const int BAUD = 9600;

    // DHT数据线使用Arduino的2号引脚
    static const int DHTPIN = 2;
    // DHT11类型，可以根据需要为DHT11,22,33
    static const int DHTTYPE = 11;

    // LCD 显示屏的I2C从端装置位址
    static const int LCD_ADDRESS = 0x27;

    // arduino的LED pin。
    static const int ledPin = 13;
};

#endif // PROJECT_CONFIG_H

