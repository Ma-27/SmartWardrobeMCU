/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 10:02
 */


#ifndef CORE_CONTROLLER_BUILDER_H
#define CORE_CONTROLLER_BUILDER_H

#include "CoreController.h"


/**
 * **核心控制单元** (`CoreController`)的类实例构建器。
    -  采用了Builder模式按照任务生产带有不同控制器实例的Controller对象
 */
class CoreControllerBuilder {
    HardwareAbstraction* hardware = nullptr;
    NetworkManager* network = nullptr;
    DataManager* data = nullptr;

public:
    CoreControllerBuilder &setHardwareAbstraction(HardwareAbstraction *hardwareAbstraction) {
        this->hardware = hardwareAbstraction;
        return *this;
    }

    CoreControllerBuilder &setNetworkManager(NetworkManager *networkManager) {
        this->network = networkManager;
        return *this;
    }

    CoreControllerBuilder &setDataManager(DataManager *dataManager) {
        this->data = dataManager;
        return *this;
    }

    CoreController* build() {
        return new CoreController(hardware, network, data);
    }

};


#endif // CORE_CONTROLLER_BUILDER_H

