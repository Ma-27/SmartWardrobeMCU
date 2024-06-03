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
    handlerMap["Pong"] = [this](const JsonObject &doc) { this->handlePing(doc); };

    // 处理错误和异常报文
    handlerMap["Error"] = [this](const JsonObject &doc) { this->handleError(doc); };

    // 对于未定义的报文类型，使用handleDefault方法
    handlerMap["Confirm"] = [this](const JsonObject &doc) { this->handleDefault(doc); };

    // 对于请求Arduino数据的报文，使用handleRequestData方法
    handlerMap["Request-Data"] = [this](const JsonObject &doc) { this->handleRequestData(doc); };

    // 处理设备登录确认报文，并且做一些配置工作
    handlerMap["Register-Ack"] = [this](const JsonObject &doc) { this->handleRegisterAck(doc); };

    // 处理接收到的命令报文
    handlerMap["Command"] = [this](const JsonObject &doc) { this->handleCommand(doc); };
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
    const int deviceId = doc["device_id"];
    if (deviceId != ProjectConfig::DEVICE_ID) {
        dataManager->logData("Device ID does not match", true);
    }

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

// 处理Pong返回报文
bool PacketParser::handlePing(const JsonObject &doc) {
    DataManager *dataManager = DataManager::getInstance();
    String dataRef = "This is data from Arduino - testing server";

    // 提取runtime
    String data = doc["data"];

    // 提取data对象
    if (data == dataRef) {
        dataManager->logData("Ping successful", true);
        return true;
    } else {
        dataManager->logData("Ping is not successful", true);
        return false;
    }
}

// 处理异常和错误报文
void PacketParser::handleError(const JsonObject &doc) {
    DataManager *dataManager = DataManager::getInstance();

    // 提取runtime
    String errorCode = doc["error_code"];
    String errorMsg = doc["error_message"];

    // 记录错误信息
    dataManager->logData("Error code: " + errorCode, true);
    dataManager->logData("Error message: " + errorMsg, true);
}


// 处理默认报文的方法
bool PacketParser::handleDefault(const JsonObject &doc) {
    DataManager *dataManager = DataManager::getInstance();

    String data = doc["data"];
    String remark = doc["remark"];

    if (data == "server received data") {
        dataManager->logData("Confirmed: server received data", true);
        dataManager->logData("Additional information: ", true);
        dataManager->logData(remark, true);
        return true;
    } else {
        dataManager->logData("receiving data from server but data cannot be confirmed", true);
        return false;
    }
}


// 处理来自服务器的请求数据报文
bool PacketParser::handleRequestData(const JsonObject &doc) {
    DataManager *dataManager = DataManager::getInstance();
    String packetType = doc["packet_type"];  // 报文类型
    String dataType = doc["data_type"];  // 请求的数据类型
    String remark = doc["remark"];  // 备注信息

    // 日志记录收到的请求
    dataManager->logData("Received request for " + dataType, true);
    dataManager->logData("Remark: " + remark, true);

    // 根据请求的数据类型，采集对应的数据
    if (dataType == "Temperature-Humidity") {
        // 使用CommandManager去调度执行命令
        CommandManager::getInstance()->parseCommand("net -dht");
        return true;
    } else if (dataType == "Light") {
        // 使用CommandManager去调度执行命令
        CommandManager::getInstance()->parseCommand("net -light");
        return true;
    } else {
        dataManager->logData("requested data type not supported: " + dataType, true);
        return false;
    }
}


// 处理设备登录确认报文
bool PacketParser::handleRegisterAck(const JsonObject &doc) {
    DataManager *dataManager = DataManager::getInstance();

    JsonObject config = doc["config"].as<JsonObject>();  // 配置信息对象
    long validInterval = config["valid_interval"];  // 配置的有效间隔
    String status = config["status"];  // 登录状态

    // 记录收到的报文和状态
    dataManager->logData("Received Register-Ack for device", true);
    dataManager->logData("Status: " + status + ", Valid Interval: " + String(validInterval), true);

    // 根据状态进行处理
    if (status == "success") {
        // 如果登录成功，进行相应的配置设置
        // 这里可以根据valid_interval设置一些定时任务或者计时器
        dataManager->logData("Login successful. Configured device...", true);
        return true;
    } else {
        // 如果登录不成功，处理失败逻辑
        dataManager->logData("Login failed.", true);
        return false;
    }
}


// 处理接收到的命令报文
bool PacketParser::handleCommand(const JsonObject &doc) {
    DataManager *dataManager = DataManager::getInstance();
    // 命令类型
    String commandType = doc["command"];
    // 操作指令
    String action = doc["action"];
    // 备注信息
    String remark = doc["remark"];

    // 记录收到的命令和操作
    dataManager->logData("Received command: " + commandType + ", Action: " + action, true);
    dataManager->logData("Remark: " + remark, true);

    // 根据命令类型和操作指令执行相应操作
    if (commandType == "Light-Manuel") {
        if (action == "turn_on") {
            // 调用相应函数开启灯光
            dataManager->logData("Turning on the light as per command.", true);
            // 使用CommandManager去调度执行命令
            CommandManager::getInstance()->parseCommand("act light -on");
        } else if (action == "turn_off") {
            // 调用相应函数关闭灯光
            dataManager->logData("Turning off the light as per command.", true);
            CommandManager::getInstance()->parseCommand("act light -off");
        }
        return true;
    }
        /// 亮度控制
    else if (commandType == "Auto-Light-Control") {
        if (action == "enable") {
            // 启用自动灯光控制
            dataManager->logData("Enabling automatic light control.", true);
            CommandManager::getInstance()->parseCommand("act light -auto");
        } else if (action == "disable") {
            // 禁用自动灯光控制
            dataManager->logData("Disabling automatic light control.", true);
            CommandManager::getInstance()->parseCommand("act light -manuel");
        }
        return true;
    }
        /// 温度控制
    else if (commandType == "Temperature-Control") {
        if (action == "enable") {
            // 启用自动温度控制
            // 目标温度
            int target = doc["target"];
            dataManager->logData("Enabling automatic temperature control,target temperature:" + String(target), true);
            CommandManager::getInstance()->parseCommand("act auto-control-temperature -tv " + String(target));
        } else if (action == "disable") {
            // 禁用自动控制
            dataManager->logData("Disabling automatic temperature control.", true);
            CommandManager::getInstance()->parseCommand("act manual-control-temperature");
        }
        return true;
    }
        /// 湿度控制
    else if (commandType == "Humidity-Control") {
        if (action == "enable") {
            // 启用自动湿度控制
            // 目标湿度
            int target = doc["target"];
            dataManager->logData("Enabling automatic humidity control,target humidity:" + String(target), true);
            CommandManager::getInstance()->parseCommand("act auto-control-humidity -hv " + String(target));
        } else if (action == "disable") {
            // 禁用自动灯光控制
            dataManager->logData("Disabling automatic humidity control.", true);
            CommandManager::getInstance()->parseCommand("act manual-control-humidity");
        }
        return true;
    }
        /// 执行器控制
    else if (commandType == "Actuator-Control") {
        // 提取执行器的名字，派发给不同的执行器
        String actuator = doc["actuator"];

        // 开闭Cooler
        if (actuator == "Cooler") {
            // 确定是开启还是关闭
            if (action == "turn_on") {
                // 提取开度值 todo 还可以增加幅度值命令解析
                int target = doc["target"];

                dataManager->logData("Enable cooler, target:" + String(target), true);
                CommandManager::getInstance()->parseCommand("act cool -on");
            } else if (action == "turn_off") {
                dataManager->logData("Disable cooler", true);
                CommandManager::getInstance()->parseCommand("act cool -off");
            } else {
                dataManager->logData("Unrecognized actuator command in packet parser-cooler.", true);
                return false;

            }
        }
            // 开闭加热器
        else if (actuator == "Heater") {
            // 确定是开启还是关闭
            if (action == "turn_on") {
                dataManager->logData("Enable heater", true);
                CommandManager::getInstance()->parseCommand("act heat -on");
            } else if (action == "turn_off") {
                dataManager->logData("Disable heater", true);
                CommandManager::getInstance()->parseCommand("act heat -off");
            } else {
                dataManager->logData("Unrecognized actuator command in packet parser-heater.", true);
                return false;
            }
        }
            // 开闭加湿器
        else if (actuator == "Humidifier") {
            // 确定是开启还是关闭
            if (action == "turn_on") {
                dataManager->logData("Enable humidifier", true);
                CommandManager::getInstance()->parseCommand("act humidify -on");
            } else if (action == "turn_off") {
                dataManager->logData("Disable humidifier", true);
                CommandManager::getInstance()->parseCommand("act humidify -off");
            } else {
                dataManager->logData("Unrecognized actuator command in packet parser-humidifier.", true);
                return false;
            }
        }
            // 开闭除湿器
        else if (actuator == "Dehumidifier") {
            // 确定是开启还是关闭
            if (action == "turn_on") {
                // 提取开度值 todo 还可以增加幅度值命令解析
                int target = doc["target"];

                dataManager->logData("Enable Dehumidifier, target:" + String(target), true);
                CommandManager::getInstance()->parseCommand("act dehumidify -on");
            } else if (action == "turn_off") {
                dataManager->logData("Disable dehumidifier", true);
                CommandManager::getInstance()->parseCommand("act dehumidify -off");
            } else {
                dataManager->logData("Unrecognized actuator command in packet parser-dehumidifier.", true);
                return false;
            }
        }
            // 衣架控制
        else if (actuator == "Shelf") {
            // 如果命令是取衣物
            if (action == "fetch") {
                dataManager->logData("shelf fetch", true);

                // 获取衣物对象
                Cloth *newCloth = getCloth(doc);
                dataManager->logData(newCloth->output(), true);

                // 根据衣物ID查找衣物所在位置
                int position = ClothManager::getInstance()->findClothPositionById(newCloth->getId());

                // 如果衣物在衣柜中,就直接取出
                if (position != -1) {
                    CommandManager::getInstance()->parseCommand("act shelf -pick_cloth " + String(position));
                } else {
                    dataManager->logData("Cloth not found in closet", true);
                }
            }
                // 如果命令是归位衣物
            else if (action == "homing") {
                dataManager->logData("shelf homing", true);

                // 获取衣物对象
                Cloth *newCloth = getCloth(doc);
                dataManager->logData(newCloth->output(), true);

                // 使用平方取中法，查找并且选定一个空位
                int position = ClothManager::getInstance()->findEmptyPositionUsingHash();
                dataManager->logData(&"adding cloth to position"[position], true);

                // 如果空位有效，则直接插入衣物
                if (position != -1) {
                    ShelfManager::getInstance()->addClothing(position, *newCloth);
                } else {
                    dataManager->logData("Cloth not found in closet", true);
                }
            } else {
                dataManager->logData("Unrecognized actuator command in packet parser-dehumidifier.", true);
                return false;
            }
        }

            // 如果什么执行器都没有匹配到，记录错误
        else {
            dataManager->logData("Unrecognized actuator in packet parser.", true);
            return false;
        }

        return true;
    }

        // 如果没有匹配的命令类型，记录错误并返回false
    else {
        dataManager->logData("Unrecognized command type in packet parser.", true);
        return false;
    }
    return true;
}


// 解包报文，创建一个新的衣物对象
Cloth *PacketParser::getCloth(const JsonObject &doc) const {
    JsonObject cloth = doc["cloth"];
    String id = cloth["id"];
    String color = cloth["color"];
    String style = cloth["style"];
    String material = cloth["material"];
    String size = cloth["size"];
    bool isInCloset = cloth["isInCloset"];
    int hangPosition = cloth["hangPosition"];
    String brand = cloth["brand"];
    String purchaseDate = cloth["purchaseDate"];
    bool isClean = cloth["isClean"];
    String lastWornDate = cloth["lastWornDate"];

    // 创建新的Cloth对象
    Cloth *newCloth = new Cloth(id, color, style, material, size, isInCloset, hangPosition, brand, purchaseDate,
                                isClean, lastWornDate);
    return newCloth;
}



