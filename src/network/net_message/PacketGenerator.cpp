/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/1 下午 09:13
 */

// PacketGenerator.cpp
#include "network/net_message/PacketGenerator.h"

PacketGenerator::PacketGenerator() {
    // NOT YET IMPLEMENTED
}

String PacketGenerator::generateTemperatureHumidityMessage(float temperature, float humidity) {
    // 分配足够的空间存储JSON文档
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    String message;
    serializeJson(doc, message); // 将JSON文档序列化为字符串
    return message;
}

String PacketGenerator::ping() {
    StaticJsonDocument<100> doc; // 分配足够的空间存储JSON文档
    doc["device_id"] = ProjectConfig::DEVICE_ID;
    doc["message_type"] = "Ping";
    String message;
    serializeJson(doc, message); // 将JSON文档序列化为字符串
    return message;
}


