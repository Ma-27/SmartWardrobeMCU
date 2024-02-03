//
// Created by Mamh on 2024/2/3.
//


#include "CoreController.h"
#include "CoreControllerBuilder.h"
#include "ProjectConfig.h"

CoreController *CoreController::instance = nullptr;

CoreController *CoreController::getInstance() {
    if (instance == nullptr) {
        // 按照builder模式实例化CoreController
        CoreControllerBuilder builder;
        instance = builder
                .setHardwareAbstraction(HardwareAbstraction::getInstance())
                .setNetworkManager(NetworkManager::getInstance())
                .setDataManager(DataManager::getInstance())

                .build();
    }
    return instance;
}

// 负责更新温湿度，此过程由HardwareAbstraction层进行。此层管理该功能是否启用。
void CoreController::updateTemperatureAndHumidity(int updateFreq) {
    // 延迟规定时间再更新
    delay(updateFreq);

    // 获取到硬件抽象层实例，并且使用硬件抽象层更新温湿度，不经过此层控制
    hardware->processTemperatureAndHumidity(true);
}

//核心控制单元中，可以一直运行的函数。它负责管理整个循环
void CoreController::looper() {
    // 更新温湿度
    updateTemperatureAndHumidity(ProjectConfig::LOOPER_UPDATE_TIME);
}

