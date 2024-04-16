/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/4 上午 10:05
 */

#ifndef PacketParser_h
#define PacketParser_h

#include <functional>
#include <map>
#include <WString.h>

#define ARDUINOJSON_ENABLE_STD_STRING 0
#define ARDUINOJSON_ENABLE_STD_STREAM 0
#include <ArduinoJson.h>


class PacketParser {
public:
    // 构造方法
    PacketParser();

    /// 带有模式指示的构造函数。在这种模式下，报文返回全部或者部分Json Object数组，而不是去解析它们并且发出指令。
    PacketParser(int mode);

    // 所有初始化工作需要在这里进行。在构造函数中进行无效。
    void initialize(int mode = PACKET_EXECUTE);

    // 报文类型常量
    static const int PACKET_EXECUTE = 0;
    static const int PACKET_RETURN_DOC = 1;

    // 解析报文的方法
    JsonDocument parsePacket(const String &jsonPacket);

    // 处理默认报文的方法
    bool handleDefault(const JsonObject &doc);

private:
    // 用于控制parsePacket行为的模式
    int parseMode;

    /** 初始化报文处理函数映射
     * 它在handlerMap中查找这个报文类型对应的处理函数。如果找到了，就执行这个函数；如果没有找到，就打印一个错误消息。
        这个过程使得每种类型的报文都能被映射到一个专门的处理函数上，这些函数可以是成员函数、普通函数或Lambda表达式。
     */
    std::map<String, std::function<void(const JsonObject &)>> handlerMap;

    void initializeHandlerMap();

    // 用于处理不同类型报文的私有方法
    bool handlePing(const JsonObject &doc);

    // 处理网络通信的报文异常与错误
    void handleError(const JsonObject &doc);

    // 处理请求数据的报文
    bool handleRequestData(const JsonObject &doc);

    // 处理注册确认报文
    bool handleRegisterAck(const JsonObject &doc);
};

#endif // PacketParser_h
