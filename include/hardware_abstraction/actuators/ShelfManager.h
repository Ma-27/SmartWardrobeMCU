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
#include "data/cloth/Cloth.h"

class ShelfManager : public CommandListener {
private:
    // 静态私有实例指针
    static ShelfManager *instance;

    // 衣架舵机
    Servo servo;

    // 初始化步进电机(放在cpp)
    // IN1, IN2, IN3, IN4, 总的步数, 每步的延迟(毫秒)
    Unistep2 stepper = Unistep2(ProjectConfig::MOTOR_IN1, ProjectConfig::MOTOR_IN2, ProjectConfig::MOTOR_IN3,
                                ProjectConfig::MOTOR_IN4, 4096, 4096);

    // 舵机默认垂下时应该转到的角度
    static const int SERVO_DEFAULT_ANGLE = -90;

    // 舵机最高转到的角度,转到这个角度取得衣物
    static const int SERVO_PICK_ANGLE = 90;

    // 舵机挂衣服时的预备角度（舵机此时携带衣物）
    static const int SERVO_CARRY_ANGLE = 45;

    // 舵机移动时的角度（舵机此时不携带衣物）
    static const int SERVO_MOVE_ANGLE = 0;

    // 舵机移动1cm时对应的步数
    static const int STEP_LENGTH_1CM = 910;

    // 使用标志物进行简单的互斥
    bool isArmMoved = false;
    // 是否正在移动？调度的时候，很可能并没有在移动。
    bool isInMoving = false;

    // 任务ID
    int taskID = 0;
    int commandLineTaskID;

    struct ShelfPosition {
        bool isEmpty;        // 位置是否为空
        Cloth *cloth;        // 挂在该位置的衣物,指针
        int positionSteps;   // 步进电机的步数，对应该位置
    };

    // 定义衣架位置数组和当前机械臂位置
    ShelfPosition positions[ProjectConfig::SHELF_NUMS];  // 自然而然的，默认位置和索引0对应
    // 机械臂当前位置的数组索引
    int currentArmPosition = 0;

    // 私有化构造函数
    ShelfManager();

    // 初始化舵机
    void initShelfManager();

    // 初始化衣架的每个位置
    void initShelfPositions();

    /** 从将舵机移动到衣架的位置。注意times变量，有times的是整体取衣物时的回调函数，这个因为中断了进程了
     *
     * @param index 要移动到的衣架的位置
     * @param flag 0: 复位，1: 移动到特定位置，并且挂衣服 2: 移动到特定位置，取下衣服
     */
    void moveToPosition(int index, int flag);
    void moveToPosition(int index);

    // 设置舵机角度
    void setServoAngle(int angle);

    // 到特定的位置取衣物
    void pickClothing(int position);

    // 添加衣物到指定位置
    void addClothing(int position, Cloth &cloth);

    // 更新特定衣架的信息
    void updateClothingStatus(int index, Cloth *cloth, bool isEmpty);

    // 让步进电机持续转动
    void rollStepper();

    /// 辅助类函数
    // 取下衣服
    void takeDownClothing();

    // 用机械臂添加衣物
    void addUpClothing();

    // 移动机械臂到原来位置
    void servoToDefaultPosition();

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    ShelfManager(const ShelfManager &) = delete;

    void operator=(const ShelfManager &) = delete;

    // 提供一个公共的访问方法
    static ShelfManager *getInstance();

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // SHELF_MANAGER_H

