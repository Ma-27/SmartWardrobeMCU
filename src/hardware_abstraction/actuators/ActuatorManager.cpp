/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:26
 */

#include "hardware_abstraction/actuators/ActuatorManager.h"

// 初始化静态实例指针
ActuatorManager *ActuatorManager::instance = nullptr;

// 提供一个公共的访问方法
ActuatorManager *ActuatorManager::getInstance() {
    if (instance == nullptr) {
        instance = new ActuatorManager();
    }
    return instance;
}

// 初始化所有执行器组件，这是该类的默认构造函数
ActuatorManager::ActuatorManager() {
    // FIXME: 执行器初始化逻辑
}