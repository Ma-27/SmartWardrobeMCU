/**
 * @description: 衣架管理类，模拟自动取衣物和归类衣物。
 * 使用步进电机前进到一个固定的位置（模拟衣物挂载的位置），舵机的角度控制模拟机械臂取物。
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/4 下午 07:39
 */

#ifndef SHELF_MANAGER_H
#define SHELF_MANAGER_H

#include <Arduino.h>
#include <Servo.h>
#include <Unistep2.h>
#include "core/CommandListener.h"
#include "utility/ProjectConfig.h"

class ShelfManager : public CommandListener {
private:
    // 静态私有实例指针
    static ShelfManager *instance;

    // 衣架舵机
    Servo servo;

    // 初始化步进电机
    // IN1, IN2, IN3, IN4, 总的步数, 每步的延迟(毫秒)
    Unistep2 stepper = Unistep2(ProjectConfig::MOTOR_IN1, ProjectConfig::MOTOR_IN2, ProjectConfig::MOTOR_IN3,
                       ProjectConfig::MOTOR_IN4, 4096, 4096);

    // 私有化构造函数
    ShelfManager();

    // 初始化舵机
    void initShelfManager();

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    ShelfManager(const ShelfManager &) = delete;

    void operator=(const ShelfManager &) = delete;

    // 提供一个公共的访问方法
    static ShelfManager *getInstance();

    // 设置舵机角度
    void setServoAngle(int angle);

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;

    // 设置步进电机前进或者后退的步数
    void setStep(int step);
};

#endif // SHELF_MANAGER_H

