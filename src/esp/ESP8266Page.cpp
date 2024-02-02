//为esp8266单独烧录的Wi-Fi 服务器代码，它显示特定的网页

/**
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "mamh"      // 路由器的名字
#define STAPSK  "mamh2015"  // 密码
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);  // 创建Web服务器，端口80

// 访客访问主页时显示的内容
void handleRoot() {
    server.send(200, "text/plain", "Hello From ESP8266 !");
}

// 找不到页面时显示的内容
void handleNotFound() {
    server.send(404, "text/plain", "File Not Found");
}

void setup(void) {
    Serial.begin(115200);  // 开始串行通信
    WiFi.mode(WIFI_STA);  // 设置为WIFI客户端模式
    WiFi.begin(ssid, password);  // 连接到WiFi
    Serial.println("");

    // 等待WiFi连接
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  // 显示获取的IP地址

    // 启动mDNS服务
    if (MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    // 绑定主页触发的副程序
    server.on("/", handleRoot);

    // 绑定找不到页面时触发的副程序
    server.onNotFound(handleNotFound);

    // 启动服务器
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();  // 处理客户端请求
    MDNS.update();  // 更新mDNS
}
*/