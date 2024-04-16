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
    success = executeAT_Setting("AT", "", 2000) && success;
    // 增加进度条，在屏幕上显示
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 设置工作模式。1：station模式；2：ap模式；3：ap+station模式。启用自动连接
    success = executeAT_Setting("AT+CWMODE=3", "OK", 1000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 加入当前Wi-Fi热点无线网络
    success = executeAT_Setting("AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"", "WIFI CONNECTED", 20000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 调试发现下一条指令在响应前一条指令中，故delay 3.5s
    delay(3500);

    // 查询本机IP和MAC地址
    success = executeAT_Setting("AT+CIFSR", "", 2000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // FIXME 这里曾经有报错，但是调试发现不影响，它并不会返回OK。
    // 连接服务器，这里TCP为TCP透传、183.230.40.33为服务器IP地址，80为端口号
    success = executeAT_Setting("AT+CIPSTART=\"TCP\",\"183.230.40.40\",1811", "AT+CIPSTART=\"TCP\"", 3000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    /// AT+CIPMUX=1 1：开启多连接；0：单链接 Warning:CIPMUX and CIPSERVER must be 0。
    /// 关键词可能是OK或者link is builded。这是实验结果。这里为了打印日志，就不验证了
    success = executeAT_Setting("AT+CIPMUX=0", "", 2000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 设置透传模式。0非透传模式；1透传模式. warning:只有普通传输模式(AT+CIPMODE=0)，才能设置为多连接
    success = executeAT_Setting("AT+CIPMODE=1", "OK", 2000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    // 尝试发送AT+CIPSEND指令
    success = executeAT_Setting("AT+CIPSEND", "OK", 5000) && success;
    dataManager->connectingProgress = dataManager->connectingProgress + 10;
    displayManager->displayProgressBar(dataManager->connectingProgress, 1);

    return success;
}

/**
 * 获得服务器的握手信息，如果显示连接上了，那就ok了
 * @return 返回是否成功
 */
bool ServerConnector::readServerShakehands() {
    // 丢弃无用数据，清空串口。监控Serial1串口，等待'>'字符
    while (true) {
        if (Serial1.available()) {
            char c = Serial1.read(); // 读取一个字符
            if (c == '>') {
                String trash = Serial1.readString();
                // 如果字符是'>'，代表ESP01S已经准备好
                break; // 跳出循环，继续执行后续代码
            }
        }
    }

    dataManager->logData("Waiting for server response...", true);

    // 向OpenIot（中国移动物联网平台）发送身份识别信息：私钥
    Serial1.println(IOT_PLATFORM_PRIVATE_KEY);


    // 用来标记是否获得服务器端预期响应
    bool success = false;
    // 获取当前时间
    unsigned long startTime = millis();
    // 设置超时时间，例如1秒
    unsigned long timeout = 1000;

    // 用于存储从服务器接收到的数据
    const int bufferSize = ProjectConfig::BUFFER_SIZE; // 1.5KB
    char tempSting[bufferSize] = {0}; // 初始化所有元素为0

    // 当前填充到tempResponse中的位置
    int currentIndex = 0;

    // 检查是否有数据可读，或者是否超时
    while ((millis() - startTime) < timeout && currentIndex < bufferSize - 1) {
        if (Serial1.available()) { // 如果有数据可读
            // 读取一个字符
            char c = Serial1.read();

            // 将字符添加到响应字符串中，确保不会溢出
            tempSting[currentIndex++] = c;
        }
    }

    // 确保字符串正确结束
    tempSting[currentIndex] = '\0';

    // 将字符数组转换为String对象
    String response = String(tempSting);


    // 如果接收到了响应
    if (response.length() > 0) {
        response.trim();

        dataManager->logData("------------This is the shakeHand data receiving ------------", false);
        dataManager->logData(response, false);
        dataManager->logData("-------------------------------------------------------------", false);

        // 检查是否接收到结束标志，确认标志是confirm报文（默认报文）
        // 这里需要根据服务器的响应格式来决定何时结束读取
        // 这里委托给NetworkDataHandler解析报文，并且返回判断结果
        if (networkDataHandler->validatePacket(response, NetworkPacketType::CONFIRM)) {
            success = true;
        }

    } else {
        // 超时，没有接收到响应
        dataManager->logData("No response from server", true);
    }

    if (success) {
        dataManager->logData("-----------------Device successfully online.-----------------", true);
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




