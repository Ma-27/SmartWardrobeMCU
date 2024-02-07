//
// Created by Mamh on 2024/2/6.
//

// NetworkStatusMessage.h
#ifndef NETWORK_STATUS_MESSAGE_H
#define NETWORK_STATUS_MESSAGE_H

#include "data/pub-sub/Message.h"

enum class ConnectionStatus {
    NotConnected = 0,
    ConnectingToWiFi = 1,
    WiFiConnected = 2,
    ServerConnected = 3
};


class NetworkStatusMessage : public Message {
private:
    // 状态用一个枚举类的对象表示
    ConnectionStatus status;
public:
    NetworkStatusMessage(ConnectionStatus status) : status(status) {

    }


    ConnectionStatus getStatus() const {
        return status;
    }
};

#endif
