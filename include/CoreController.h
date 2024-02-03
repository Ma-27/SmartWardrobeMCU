//
// Created by Mamh on 2024/2/2.
//

#ifndef CORE_CONTROLLER_H
#define CORE_CONTROLLER_H

#include "HardwareAbstraction.h"
#include "SerialManager.h"
#include "NetworkManager.h"
#include "DataManager.h"

/**
 * **核心控制单元** (`CoreController`)
    - 负责整体的业务逻辑控制。包括控制中断，初始化，循环体等等。
    - 协调传感器读取、命令解析、状态管理和网络通信。
 */



class CoreControllerBuilder; // 前向声明

class CoreController {

private:
    // 自身实例，采用单例模式，只有自己可见自己的实例。通过getInstance();方法被需要的类调用
    static CoreController* instance;


    // 硬件抽象对象，有了这个对象可以调用任何硬件功能
    HardwareAbstraction* hardware;
    // 网络抽象对象，有了这个对象可以进行各种网络操作
    NetworkManager* network;
    // 数据抽象对象，有了这个对象可以对数据进行处理
    DataManager* data;


    // 私有构造函数防止外部直接实例化
    CoreController(HardwareAbstraction* hardware, NetworkManager* network, DataManager* data)
            : hardware(hardware), network(network), data(data) {

    }

    // 允许CoreControllerBuilder访问CoreController的私有部分
    friend class CoreControllerBuilder;

public:
    // 获取单例对象的静态方法
    static CoreController* getInstance();

    // 更新温湿度
    void updateTemperatureAndHumidity(int updateFreq);

    // 直接来自于loop函数的looper。此函数循环往复，一直运行
    void looper();

    // Getter方法
    HardwareAbstraction* getHardwareAbstraction() const {
        return hardware;
    }

    NetworkManager* getNetworkManager() const {
        return network;
    }

    DataManager* getDataManager() const {
        return data;
    }
};

#endif


