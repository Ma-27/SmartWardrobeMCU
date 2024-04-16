/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/4/4 上午 09:20
 */

#ifndef NetworkPacketType_h
#define NetworkPacketType_h

class NetworkPacketType {
public:
    /// 和云平台通信的上传消息类型

    // Ping 命令
    static const int PING = 1;


    /// 和云平台通信的下载消息类型

    // Pong 命令
    static const int PONG = 65534;

    // 服务器确认报文已经收到（见protocol.md 文档中 1. ）
    static const int CONFIRM = 65533;


};

#endif //NetworkPacketType_h
