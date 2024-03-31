/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/21 下午 08:56
 */

#include "network/ServerConnector.h"
#include "utility/ProjectConfig.h"

ServerConnector *ServerConnector::instance = nullptr;

ServerConnector *ServerConnector::getInstance() {
    if (instance == nullptr) {
        instance = new ServerConnector();
    }
    return instance;
}


// 初始化所有这个类要用到的组件，这是该类的默认构造函数
ServerConnector::ServerConnector() {
    // 获得DataManager实例
    dataManager = DataManager::getInstance();

    // 获得DisplayManager实例
    displayManager = DisplayManager::getInstance();
}


// 执行AT测试和AT命令连接
bool ServerConnector::executeAT_Setting(const char *data, const char *keyword, unsigned long time_out) {
    // 打印接收到的每行数据
    dataManager->logData("Starting Command: " + String(data), false);

    // 使用Serial1连接Arduino和esp01
    Serial1.println(data);
    Serial1.flush();
    // 延时以等待模块响应
    delay(100);

    unsigned long start = millis();
    String readData = "";

    while (millis() - start < time_out) {
        if (Serial1.available()) {
            readData = Serial1.readStringUntil('\n');
            // 打印接收到的每行数据
            dataManager->logData("Received: " + readData, false);
            if (readData.indexOf(keyword) >= 0) {
                dataManager->logData("-----------------Received expected response:" + readData + "-----------------",
                                     false);   // 收到预期响应
                return true;                                                // 找到关键词
            }
        }
    }

    // 使用Serial打印超时调试信息
    dataManager->logData("-----------------Time Out for ERROR: " + String(data) + "-----------------", true);
    return false;
}

/**
 * 连接到指定的Wi-Fi接入点AP，接入互联网
 */
bool ServerConnector::configWifiSettings() {
    bool success = true;
    // 开始连接wifi
    dataManager->logData("Wi-Fi Connecting", false);

    // AT 测试 esp8266能否工作
    success = executeAT_Setting("AT", "OK", 2000) && success;
    // 增加进度条，在屏幕上显示
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 设置工作模式。1：station模式；2：ap模式；3：ap+station模式
    success = executeAT_Setting("AT+CWMODE=3", "OK", 1000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 加入当前Wi-Fi热点无线网络
    success = executeAT_Setting("AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"", "WIFI CONNECTED", 20000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 调试发现下一条指令在响应前一条指令中，故delay 3.5s
    delay(3500);

    // 查询本机IP
    success = executeAT_Setting("AT+CIFSR", "OK", 2000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // FIXME 这里曾经有报错，但是调试发现不影响，它并不会返回OK。
    // 连接服务器，这里TCP为TCP透传、183.230.40.33为服务器IP地址，80为端口号
    success = executeAT_Setting("AT+CIPSTART=\"TCP\",\"183.230.40.40\",1811", "AT+CIPSTART=\"TCP\"", 3000) &&
              success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // AT+CIPMUX=1 1：开启多连接；0：单链接 Warning:CIPMUX and CIPSERVER must be 0。
    // FIXME OK  关键词可能是OK。这是实验结果
    success = executeAT_Setting("AT+CIPMUX=1", "link is builded", 2000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 设置透传模式。0非透传模式；1透传模式
    success = executeAT_Setting("AT+CIPMODE=1", "OK", 2000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 尝试发送AT+CIPSEND指令
    success = executeAT_Setting("AT+CIPSEND", "OK", 5000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 向OpenIot（中国移动物联网平台）发送身份识别信息：私钥
    Serial1.println(IOT_PLATFORM_PRIVATE_KEY);

    return success;
}

/**
 * 获得服务器的握手信息，如果显示连接上了，那就ok了
 * @return 返回是否成功
 */
bool ServerConnector::readServerShakehands() {
    // 用来标记是否获得服务器端预期响应
    bool success = false;
    // 获取当前时间
    unsigned long startTime = millis();
    // 设置超时时间，例如5秒
    unsigned long timeout = 5000;
    // 用于存储从服务器接收到的数据
    String response = "";
    // 提示信息
    dataManager->logData("Waiting for server response...", false);

    // 检查是否有数据可读，或者是否超时
    while ((millis() - startTime) < timeout) {
        if (Serial1.available()) { // 如果有数据可读
            // 读取一个字符
            char c = Serial1.read();
            // 将字符添加到响应字符串中
            response += c;
        }

        // 检查是否接收到结束标志，例如，某些特定的字符或字符串
        // 这里需要根据服务器的响应格式来决定何时结束读取
        // 例如，如果服务器在消息结束时发送特定字符串，如"END"，则可以这样检查：
        if (response.endsWith("received")) {
            success = true;
            // 已经获得预期响应了，跳出循环
            break;
        }
    }

    // 如果接收到了响应
    if (response.length() > 0) {
        dataManager->logData("Received response from server:", false);
        // 打印响应
        dataManager->logData(response, false);
    } else {
        // 超时，没有接收到响应
        dataManager->logData("No response from server (timeout)", true);
    }

    if (success) {
        dataManager->logData("-----------------Device successfully online.-----------------", false);
    }
    return success;
}

// 从服务器断开连接
bool ServerConnector::disconnectFromServer() {
    return executeAT_Setting("AT+CWQAP", "OK", 2000);
}


// 测试AT+RST 复位连接
bool ServerConnector::resetConnection() {
    // 增加超时时间
    return executeAT_Setting("AT+RST", "OK", 5000);
}

// 测试AT+RST 复位连接
bool ServerConnector::testMQTT() {
    // 增加超时时间
    return executeAT_Setting("AT+MQTTUSERCFG?", "", 5000);
}




