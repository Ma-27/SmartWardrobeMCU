/**
 * @description: 执行器管理器** (`ActuatorManager`)
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:26
 */


#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include "data/pub-sub/Message.h"
#include "data/pub-sub/EventManager.h"
#include "hardware_abstraction/actuators/Light.h"

/**
 * **执行器管理器** (`ActuatorManager`)
    - 负责管理所有执行器，如LED灯、步进电机等等可以被控制驱动的电子单元。
    - 根据核心控制单元的指令执行动作。
 */
class ActuatorManager : public Subscriber {
private:
    static ActuatorManager *instance; // 静态私有实例指针

    // 私有化构造函数
    ActuatorManager();

    /**
    * 实现Subscriber接口要求的update方法。
    *
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;

    void initActuatorManager();

    // 保存事件接收器的一个对象
    EventManager *eventManager;

    // 灯光控制类
    Light *light;

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    ActuatorManager(const ActuatorManager &) = delete;

    void operator=(const ActuatorManager &) = delete;

    // 提供一个公共的访问方法
    static ActuatorManager *getInstance();

    // 设置灯光强度
    void setLightIntensity(int intensity);
};

#endif // ACTUATOR_MANAGER_H
