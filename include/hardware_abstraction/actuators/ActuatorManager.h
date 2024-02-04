//
// Created by Mamh on 2024/2/2.
//

/**
 * **执行器管理器** (`ActuatorManager`)
    - 负责管理所有执行器，如LED灯、步进电机等等可以被控制驱动的电子单元。
    - 根据核心控制单元的指令执行动作。
 */



#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

/**
 * 执行器管理层，负责提供执行器的接口，执行器的初始化和释放，执行器状态的监测，执行器的动作执行处理等等。
 */
class ActuatorManager {
private:
    static ActuatorManager *instance; // 静态私有实例指针
    // 私有化构造函数
    ActuatorManager();

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    ActuatorManager(const ActuatorManager &) = delete;

    void operator=(const ActuatorManager &) = delete;

    // 提供一个公共的访问方法
    static ActuatorManager *getInstance();
};

#endif // ACTUATOR_MANAGER_H
