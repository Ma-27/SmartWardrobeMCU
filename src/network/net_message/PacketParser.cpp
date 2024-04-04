/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/4 上午 10:05
 */

#include "network/net_message/PacketParser.h"
#include "data/DataManager.h"


// 默认构造函数中初始化模式为PACKET_EXECUTE, 即解析报文并执行相应的处理函数，不再进行任何初始化工作
PacketParser::PacketParser() {
    // KEEP IT EMPTY
}


// 带参数构造函数，当解析报文时，不进行命令分发而是直接返回解析后的JsonDocument
PacketParser::PacketParser(int mode) {
    // KEEP IT EMPTY
}

// 新增的初始化函数，用于执行之前在构造函数中完成的初始化工作
void PacketParser::initialize(int mode) {
    // 设置解析模式
    parseMode = mode;

    // 根据模式初始化报文处理函数散列表，只有在默认模式下才初始化
    if (mode == PACKET_EXECUTE) {
        initializeHandlerMap();
    }
}

// 初始化报文处理函数映射, 为每种报文类型添加处理函数，降低时间复杂度
void PacketParser::initializeHandlerMap() {
    // 处理Pong返回报文
    handlerMap["Ping"] = [this](const JsonObject &doc) { this->handlePing(doc); };

    // 处理温湿度数据更新报文
    handlerMap["Temperature-Humidity-Update"] = [this](const JsonObject &doc) {
        this->handleTemperatureHumidityUpdate(doc);
    };

    // 对于未定义的报文类型，使用handleDefault方法
    handlerMap["confirm"] = [this](const JsonObject &doc) { this->handleDefault(doc); };
}


// 解析报文的方法
JsonDocument PacketParser::parsePacket(const String &jsonPacket) {
    DataManager *dataManager = DataManager::getInstance();

    dataManager->logData("Parsing packet: ", false);
    dataManager->logData(jsonPacket, false);

    // 分配足够的空间来解析JSON报文
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonPacket);

    if (error) {
        dataManager->logData("deserializeJson() failed with code ", true);
        dataManager->logData(error.c_str(), true);
    }

    // 如果模式为PACKET_RETURN_DOC, 则返回解析后的JsonDocument
    if (parseMode == PACKET_RETURN_DOC) {
        return doc;
    }

    // 提取device_id
    const char *deviceId = doc["device_id"];
    // 提取packet_type
    const char *packetType = doc["packet_type"];

    auto it = handlerMap.find(packetType);
    if (it != handlerMap.end()) {
        it->second(doc.as<JsonObject>());
    } else {
        dataManager->logData("Unknown Packet type received", true);
    }

    /// 返回空的JsonDocument,但是一定不可以不返回，这里曾引发严重的Bug。
    JsonDocument emptyDoc;
    return emptyDoc;
}

void PacketParser::handlePing(const JsonObject &doc) {
    // FIXME
}

void PacketParser::handleTemperatureHumidityUpdate(const JsonObject &doc) {
    // 提取runtime
    const char *runtime = doc["runtime"];
    // 提取data对象
    JsonObject data = doc["data"].as<JsonObject>();
    // 对data对象中的键值对进行遍历和处理
    for (JsonPair kv: data) {
        Serial.print(kv.key().c_str());
        Serial.print(": ");
        Serial.println(kv.value().as<String>());
    }
}


bool PacketParser::handleDefault(const JsonObject &doc) {
    DataManager *dataManager = DataManager::getInstance();

    const char *data = doc["data"];
    if (strcmp(data, "server received data") == 0) {
        dataManager->logData("Confirmed: server received data", true);
        dataManager->logData("", true);
        return true;
    } else {
        dataManager->logData("Data not confirmed", true);
        dataManager->logData("", true);
        return false;
    }
}


