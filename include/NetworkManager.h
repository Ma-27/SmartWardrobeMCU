//
// Created by Mamh on 2024/2/2.
//

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

/**
 * 网络抽象层，负责提供网络和设备的接口，网络的连接和释放，网络状态的监测，网络的数据收发处理等等。
 */
class NetworkManager {
private:
    static NetworkManager *instance; // 静态公共私有实例指针
    // 私有化构造函数
    NetworkManager();

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    NetworkManager(const NetworkManager &) = delete;

    void operator=(const NetworkManager &) = delete;

    // 提供一个公共的访问方法
    static NetworkManager *getInstance();
};

#endif
