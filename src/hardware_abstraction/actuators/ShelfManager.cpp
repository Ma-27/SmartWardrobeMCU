/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/4 下午 07:39
 */

#include "hardware_abstraction/actuators/GarmentHangerManager.h"
#include "utility/ProjectConfig.h"

// 初始化静态实例指针
ShelfManager *ShelfManager::instance = nullptr;

// 提供一个公共的访问方法
ShelfManager *ShelfManager::getInstance() {
    if (instance == nullptr) {
        instance = new ShelfManager();
        instance->initShelfManager();
    }
    return instance;
}

// 初始化舵机
void ShelfManager::initShelfManager() {

    // 初始化舵机
    // 附加舵机到指定引脚
    servo.attach(ProjectConfig::SERVO_PIN);


    // 初始化步进电机

    // 测试，启动步进电机
    // stepper.run();
}

// 私有化构造函数
ShelfManager::ShelfManager() {
    // KEEP IT EMPTY
}

/// 设置舵机角度
void ShelfManager::setServoAngle(int angle) {
    // 限制舵机角度在0到180之间
    angle = constrain(angle, 0, 180);
    servo.write(angle);
}


// 设置步进电机前进或者后退的步数
void ShelfManager::setStep(int step) {
    stepper.move(step);
}


// ------------------------------------------------------------------------------------------------------------------------------------
// 解析命令
bool ShelfManager::parseCommand(const String &command) {
    // 移除字符串首尾的空白字符
    String trimmedCommand = command;
    trimmedCommand.trim();

    if (trimmedCommand.length() > 0) {
        // 如果命令不为空（即含有子层级的命令），递交给其子类处理
        return dispatchCommand(trimmedCommand, "", this);
    } else {
        // 如果命令为空，返回false
        return false;
    }
}

// 具体解析是哪个负责执行命令，派发给相应的监听器
bool ShelfManager::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 删除命令前的所有空格
    command.trim();

    // 去掉命令开头的"-"，以保留"-on"的格式
    String processedCommand = command.substring(1);
    processedCommand.trim();

    /// 执行命令并处理参数
    // 设定舵机转到固定角度
    if (processedCommand.startsWith("servo_angle")) {
        // 去掉参数servo_angle
        String angleString = processedCommand.substring(11);
        angleString.trim();

        // 将参数转换为整数
        int angle = angleString.toInt();

        if (angle < -360 || angle > 360) {
            Serial.println("invalid parameter for servo_angle,beyond 360.");
            return false;
        }
        // 设定角度
        setServoAngle(angle);

        // 设定步进电机的前进或者后退步数
    } else if (processedCommand.startsWith("stepper_step")) {
        // 去掉参数stepper_step
        String stepString = processedCommand.substring(12);
        stepString.trim();

        // 将参数转换为整数
        int step = stepString.toInt();


        // 设定角度
        setStep(step);

        // 启动步进电机
    } else if (processedCommand.startsWith("stepper_run")) {
        stepper.run();

    } else {
        // 不继续向下处理
        Serial.println("Unknown command in ShelfManager: " + processedCommand);
        return false;
    }
    return true;
}

