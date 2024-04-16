/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/1 下午 09:13
 */

// PacketGenerator.cpp
#include "network/net_message/PacketGenerator.h"
#include "data/DataManager.h"

PacketGenerator::PacketGenerator() {
    // NOT YET IMPLEMENTED
}

// 生成温湿度数据的JSON消息
String PacketGenerator::generateTemperatureHumidityMessage() {
    DataManager *dataManager = DataManager::getInstance();

    // 分配足够的空间存储JSON文档
    JsonDocument doc;

    doc["device_id"] = ProjectConfig::DEVICE_ID;
    doc["from"] = ProjectConfig::DEVICE_ID;
    doc["packet_type"] = "Temperature-Humidity";
    doc["data"]["temperature"] = dataManager->temperature;
    doc["data"]["humidity"] = dataManager->humidity;
    doc["data"]["measure_time"] = dataManager->TemperatureHumidityUpdateTime;
    doc["runtime"] = millis();

    String message;
    serializeJson(doc, message); // 将JSON文档序列化为字符串
    return message;
}

// 生成ping消息
String PacketGenerator::ping() {
    String data = "This is data from Arduino - testing server";
    JsonDocument doc; // 分配足够的空间存储JSON文档
    doc["device_id"] = ProjectConfig::DEVICE_ID;
    doc["from"] = ProjectConfig::DEVICE_ID;
    doc["packet_type"] = "Ping";
    doc["runtime"] = millis();
    doc["data"] = data;

    String message;
    serializeJson(doc, message); // 将JSON文档序列化为字符串
    return message;
}

// 生成温湿度数据的JSON消息群组 FIXME 未实现
String PacketGenerator::temperatureHumidityBatch() {
    DataManager *dataManager = DataManager::getInstance();

    JsonDocument doc;  // 分配足够的空间存储JSON文档

    // 填充基础信息
    doc["device_id"] = ProjectConfig::DEVICE_ID;
    doc["from"] = ProjectConfig::DEVICE_ID;
    doc["packet_type"] = "Temperature-Humidity";
    doc["runtime"] = millis();

    // 假设这些数据是从传感器中读取的
    JsonArray data = doc.createNestedArray("data");
    JsonObject tempHum1 = data.createNestedObject();
    tempHum1["temperature"] = 22.74;
    tempHum1["humidity"] = 38.12;
    tempHum1["measure_time"] = 1627875660;

    JsonObject tempHum2 = data.createNestedObject();
    tempHum2["temperature"] = 23.00;
    tempHum2["humidity"] = 37.80;
    tempHum2["measure_time"] = 1627875660;

    String message;
    serializeJson(doc, message);  // 将JSON文档序列化为字符串
    return message;
}


// 生成用户登录报文
String PacketGenerator::login() {
    DataManager *dataManager = DataManager::getInstance();

    // 分配足够的空间存储JSON文档
    JsonDocument doc;

    // 填充基础信息
    doc["device_id"] = ProjectConfig::DEVICE_ID;
    doc["from"] = ProjectConfig::DEVICE_ID;
    doc["packet_type"] = "Log-In";
    doc["runtime"] = millis();
    // TODO 未实现，需要根据实际情况填充，这里仅仅是示例
    doc["user_name"] = "Micheal";
    doc["password_hash"] = "82af731bca2cd35ea479f3f63be70a39t0f0p682";

    String message;
    serializeJson(doc, message);  // 将JSON文档序列化为字符串
    return message;
}


// 生成灯光状态报文
String PacketGenerator::generateLightMessage() {
    DataManager *dataManager = DataManager::getInstance();

    // 分配足够的空间存储JSON文档
    JsonDocument doc;

    // 填充基础信息
    doc["device_id"] = ProjectConfig::DEVICE_ID;
    doc["from"] = ProjectConfig::DEVICE_ID;
    doc["packet_type"] = "Light";
    doc["runtime"] = millis();

    // 数据部分
    JsonObject data = doc.createNestedObject("data");
    // 传感器获取的光照强度值
    data["brightness"] = dataManager->lightIntensity;
    // 用户通过旋钮手动调节的值
    data["knob_value"] = dataManager->potentiometerValue;
    // 灯光的亮度
    data["light_openness"] = dataManager->light;
    // 获取当前时间戳
    data["measure_time"] = dataManager->lightUpdateTime;

    String message;
    serializeJson(doc, message);  // 将JSON文档序列化为字符串
    return message;
}



