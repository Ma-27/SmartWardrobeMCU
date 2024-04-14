/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/1 下午 09:13
 */

// PacketGenerator.h
#ifndef MessageGenerator_h
#define MessageGenerator_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "utility/ProjectConfig.h"

class PacketGenerator {
public:
    // 生成温湿度数据的JSON消息
    static String generateTemperatureHumidityMessage();

    // 生成ping消息
    static String ping();

private:
    PacketGenerator(); // 私有化构造函数，确保不会实例化这个类
    String temperatureHumidityBatch();

    String login();
};

#endif


