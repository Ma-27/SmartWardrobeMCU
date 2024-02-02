#include <Arduino.h>
#include <HardwareSerial.h>
#include <WString.h>

// 定义WiFi名称和密码
#define WIFI_SSID "mamh"
#define WIFI_PASSWORD "mamh2015"

boolean at_exec(const char *data, const char *keyword, unsigned long time_out) {
    Serial1.println(data);  // 使用Serial1连接Arduino和esp01
    Serial1.flush();
    delay(100);  // 延时以等待模块响应

    unsigned long start = millis();
    String readData = "";

    while (millis() - start < time_out) {
        if (Serial1.available()) {
            readData = Serial1.readStringUntil('\n');
            if (readData.indexOf(keyword) >= 0) {
                Serial.println("Received expected response: " + readData);  // 收到预期响应
                return true;                                                // 找到关键词
            }
            Serial.println("Received: " + readData);  // 打印接收到的每行数据
        }
    }

    Serial.println("Time Out for: " + String(data));  // 使用Serial打印超时调试信息
    return false;                                     // 超时
}

void setup() {
    Serial.begin(9600);     // USB串口监视器的波特率
    Serial1.begin(115200);  // 用于ESP01的串口波特率

    // 开始连接wifi
    Serial.println("Wi-Fi Connecting");

    // AT 测试 esp8266能否工作
    while (!at_exec("AT", "OK", 2000));

    // 测试AT+RST 能否复位
    // while (!at_exec("AT+RST", "OK", 5000));           // 增加超时时间

    // 设置工作模式。1：station模式；2：ap模式；3：ap+station模式
    while (!at_exec("AT+CWMODE=3", "OK", 10000));

    // 断开和当前路由器的连接
    // while (!at_exec("AT+CWQAP", "OK", 2000));

    // 加入当前Wi-Fi热点无线网络
    while (!at_exec("AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"", "WIFI CONNECTED", 20000));

    // 查询本机IP
    while (!at_exec("AT+CIFSR", "OK", 1000));

    // 连接服务器，这里TCP为TCP透传、183.230.40.33为服务器IP地址，80为端口号
    while (!at_exec("AT+CIPSTART=\"TCP\",\"183.230.40.40\",1811", "CONNECT", 10000));

    // AT+CIPMUX=1 1：开启多连接；0：单链接
    while (!at_exec("AT+CIPMUX=0", "OK", 2000));

    // 设置透传模式。0非透传模式；1透传模式
    while (!at_exec("AT+CIPMODE=1", "OK", 2000));

    // 尝试发送AT+CIPSEND指令
    while (!at_exec("AT+CIPSEND", "OK", 2000));

    Serial1.println("*627051#KimirrCloset#script*");
    readServerResponse();
}

void loop() {

}

void readServerResponse() {
    unsigned long startTime = millis(); // 获取当前时间
    unsigned long timeout = 10000; // 设置超时时间，例如10秒
    String response = ""; // 用于存储从服务器接收到的数据

    Serial.println("Waiting for server response...");

    // 检查是否有数据可读，或者是否超时
    while ((millis() - startTime) < timeout) {
        if (Serial1.available()) { // 如果有数据可读
            char c = Serial1.read(); // 读取一个字符
            response += c; // 将字符添加到响应字符串中
        }

        // 检查是否接收到结束标志，例如，某些特定的字符或字符串
        // 这里需要根据服务器的响应格式来决定何时结束读取
        // 例如，如果服务器在消息结束时发送特定字符串，如"END"，则可以这样检查：
        if (response.endsWith("received")) {
            Serial.println("Device successfully online.");
            break; // 跳出循环
        }
    }

    if (response.length() > 0) { // 如果接收到了响应
        Serial.println("Received response from server:");
        Serial.println(response); // 打印响应
    } else {
        Serial.println("No response from server (timeout)"); // 超时，没有接收到响应
    }
}

