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
        instance->initActuatorManager();
    }
    return instance;
}

// 初始化所有执行器组件，这是该类的默认构造函数
ActuatorManager::ActuatorManager() {
    // KEEP IT EMPTY
}

void ActuatorManager::initActuatorManager() {
    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅NETWORK_STATUS_CHANGE消息
    // eventManager->subscribe(FIXME LATER, this);

    // 初始化灯光控制类
    light = Light::getInstance();
}

/**
* 实现Subscriber接口要求的update方法。
*
* @param message 收到的消息
* @param messageType 收到的消息类型，int类型号
*/
void ActuatorManager::update(const Message &message, int messageType) {

}

// 亮灯
void ActuatorManager::setLightIntensity(int intensity) {
    light->setLightIntensity(intensity);
}


