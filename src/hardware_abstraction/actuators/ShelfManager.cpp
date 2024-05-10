/**
 * @description: 衣架管理类，模拟自动取衣物和归类衣物。
 * 使用步进电机前进到一个固定的位置（模拟衣物挂载的位置），舵机的角度控制模拟机械臂取物。
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/4 下午 07:39
 */

#include "hardware_abstraction/actuators/ShelfManager.h"
#include "utility/ProjectConfig.h"
#include "data/DataManager.h"

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
    servo.attach(ProjectConfig::SERVO_PIN);
    // 初始设置为默认角度
    setServoAngle(SERVO_DEFAULT_ANGLE);

    // 初始化每个衣架的位置
    initShelfPositions();
}

// 私有化构造函数
ShelfManager::ShelfManager() {
    // KEEP IT EMPTY
}

// 初始化每个衣架的位置
void ShelfManager::initShelfPositions() {
    // 初始化每个衣架位置
    for (int i = 0; i < 6; i++) {
        positions[i].isEmpty = true;
        positions[i].cloth = nullptr;
        positions[i].positionSteps = i * STEP_LENGTH_1CM;  // 假设每个位置间隔1cm，转换为步进电机的步数
    }
    // 默认位置为第0位
    currentArmPosition = 0;
}

//-----------------------------------------------------------------------------------------------------------------




// 移动机械臂到指定的衣架位置
void ShelfManager::moveToPosition(int index) {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();

    if (index < 0 || index >= 6) {
        dataManager->logData("Invalid position index in moving to position.", true);
        return;
    }

    dataManager->logData("Moving to position started:" + String(index), true);

    stepper.move(positions[index].positionSteps - positions[currentArmPosition].positionSteps);
    isInMoving = true;

    currentArmPosition = index;

    // 启动步进电机，添加调度任务
    taskID = TaskScheduler::getInstance().addTask([this]() { this->rollStepper(); }, ProjectConfig::NO_INTERVAL,
                                                  "rollStepper");

    dataManager->logData("Moving to position done:" + String(index), true);
}


// 移动机械臂到指定的衣架位置
void ShelfManager::moveToPosition(int index, int flag) {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();

    if (index < 0 || index >= 6) {
        dataManager->logData("Invalid position index in moving to position.", true);
        return;
    }

    dataManager->logData("Moving to position started:" + String(index), true);

    stepper.move(positions[index].positionSteps - positions[currentArmPosition].positionSteps);

    isInMoving = true;
    currentArmPosition = index;

    // 启动步进电机，添加调度任务
    taskID = TaskScheduler::getInstance().addTask([this]() { this->rollStepper(); }, ProjectConfig::NO_INTERVAL,
                                                  "rollStepper");
    // 这里需要采用让出机制和恢复机制,让出当前任务的执行权，等待恢复
    commandLineTaskID = dataManager->commandLineTaskID;
    // 暂停命令行任务，并且等待步进电机走到特定的位置
    if (flag == 0) {
        TaskScheduler::getInstance().yield(commandLineTaskID, [this, &index]() { this->servoToDefaultPosition(); });
    } else if (flag == 1) {
        TaskScheduler::getInstance().yield(commandLineTaskID, [this]() { this->addUpClothing(); });
    } else if (flag == 2) {
        TaskScheduler::getInstance().yield(commandLineTaskID, [this, &index]() { this->takeDownClothing(); });
    } else {
        dataManager->logData("Invalid flag in moving to position.", true);
    }

    dataManager->logData("Moving to position done:" + String(index), true);
}


// 更新特定衣架的信息
void ShelfManager::updateClothingStatus(int index, Cloth *cloth, bool isEmpty) {
    DataManager *dataManager = DataManager::getInstance();
    if (index < 0 || index >= 6) {
        dataManager->logData("Invalid position index in updating clothing status.", true);
        return;
    }
    positions[index].isEmpty = isEmpty;
    positions[index].cloth = cloth; // 更新衣物引用
}


// 设置舵机角度
void ShelfManager::setServoAngle(int angle) {
    if (angle < 0) {
        // 如果传入的角度是负数，从90度基准点反向转动
        angle = 90 + angle;  // angle为负，所以实际上是90 - abs(angle)
    } else {
        // 如果传入的角度是正数，从90度基准点正向转动
        angle = 90 + angle;
    }

    // 限制舵机角度在0到180之间
    angle = constrain(angle, 0, 180);
    servo.write(angle);
}


// 用于周期性更新步进电机状态的函数
void ShelfManager::rollStepper() {
    stepper.run();

    // 如果stepsToGo=0，表示已经到达目标位置
    if (stepper.stepsToGo() == 0) {
        // 取消调度任务
        int status = TaskScheduler::getInstance().deleteTask(taskID);

        if (status == 0) { Serial.println("Stepper motor has reached the target position."); }

        // 恢复命令行任务
        TaskScheduler::getInstance().resume(commandLineTaskID);
    }
}


// 取衣物的操作函数
void ShelfManager::pickClothing(int position) {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();

    if (position < 0 || position >= 6) {
        dataManager->logData("Invalid position index in picking cloth.", true);
        return;
    }

    // 拿下衣物，如果有的话
    if (!positions[position].isEmpty) {
        // 标记位置为空
        updateClothingStatus(position, nullptr, true);

        // 移动到取衣服的位置
        setServoAngle(ShelfManager::SERVO_MOVE_ANGLE);
        moveToPosition(position, 2);

    } else {
        dataManager->logData("No cloth in position " + String(position), true);
    }
}


// 添加衣物到指定位置
void ShelfManager::addClothing(int position, Cloth &cloth) {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();
    isArmMoved = false;

    if (position < 0 || position >= 6) {
        dataManager->logData("Invalid position index in adding cloth.", true);
        return;
    }

    // 检查位置是否为空
    if (positions[position].isEmpty) {
        // 移动到添加衣物的位置
        setServoAngle(ShelfManager::SERVO_CARRY_ANGLE);

        // 将衣物引用指向传递的衣物对象
        positions[position].cloth = &cloth;
        // 标记位置非空
        positions[position].isEmpty = false;

        moveToPosition(position, 1);

    } else {
        dataManager->logData("Position " + String(position) + " is already occupied.", true);
    }
}


///   ++++++++++++++++++++++++++++++++ 辅助函数
// 用机械臂拿下衣物
void ShelfManager::takeDownClothing() {
    // 拿下衣物的操作
    delay(500);
    setServoAngle(SERVO_PICK_ANGLE);
    delay(200);
    setServoAngle(SERVO_CARRY_ANGLE);
    delay(500);

    // 返回到默认位置
    moveToPosition(0, 0);
}

// 用机械臂添加衣物
void ShelfManager::addUpClothing() {
    // 添加衣物
    delay(500);
    setServoAngle(SERVO_PICK_ANGLE);
    delay(200);
    setServoAngle(SERVO_MOVE_ANGLE);
    delay(500);

    // 返回到移动位置
    setServoAngle(SERVO_MOVE_ANGLE);

    moveToPosition(0, 0);
}


// 舵机回到默认位置
void ShelfManager::servoToDefaultPosition() {
    DataManager *dataManager = DataManager::getInstance();
    setServoAngle(SERVO_DEFAULT_ANGLE);
    // 记录添加衣物的操作
    dataManager->logData("Operation successful", true);
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
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();

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
            dataManager->logData("invalid parameter for servo_angle,beyond 360.", true);
            return false;
        }


        // 设定角度
        setServoAngle(angle);
        dataManager->logData(String(angle), true);

        // 设定步进电机的前进或者后退步数
    } else if (processedCommand.startsWith("stepper_step")) {
        // 去掉参数stepper_step
        String stepString = processedCommand.substring(12);
        stepString.trim();

        // 将参数转换为整数
        int step = stepString.toInt();

        dataManager->logData("stepper_step", true);
        dataManager->logData(String(step), true);

        // 设定角度
        stepper.move(step);
        // 启动步进电机，添加调度任务
        taskID = TaskScheduler::getInstance().addTask([this]() { this->rollStepper(); }, ProjectConfig::NO_INTERVAL,
                                                      "rollStepper specific step");

    } else if (processedCommand.startsWith("pick_cloth")) {
        // 去掉参数pick_clothing
        String trimmedPosition = processedCommand.substring(10);
        trimmedPosition.trim();

        // 将参数转换为整数
        int position = trimmedPosition.toInt();

        dataManager->logData("pick clothing in:", true);
        dataManager->logData(String(position), true);

        // 取衣物
        pickClothing(position);

    } else if (processedCommand.startsWith("moveto")) {
        // 去掉参数moveto
        String trimmedPosition = processedCommand.substring(6);
        trimmedPosition.trim();

        // 将参数转换为整数
        int position = trimmedPosition.toInt();

        dataManager->logData("move to position:", true);
        dataManager->logData(String(position), true);

        //移动到指定位置
        moveToPosition(position);

    } else if (processedCommand.startsWith("add_cloth")) {
        // 去掉参数add_cloth
        String trimmedPosition = processedCommand.substring(9);
        trimmedPosition.trim();

        // 将参数转换为整数
        int position = trimmedPosition.toInt();

        dataManager->logData("Adding cloth to position:", true);
        dataManager->logData(String(position), true);

        // 创建一个新的衣物对象(举例子）
        Cloth* newCloth = new Cloth("123", "Blue", "T-Shirt", "Cotton", "M", true, 5, "Nike", "2023-01-01", true, "2023-05-01");

        // 在指定位置添加衣物
        addClothing(position, *newCloth);

    } else if (processedCommand.startsWith("position")) {
        dataManager->logData("stepper position", true);
        dataManager->logData(String(stepper.currentPosition()), true);

        dataManager->logData("machine arm position:", true);
        dataManager->logData(String(currentArmPosition), true);

    } else if (processedCommand.startsWith("view_position")) {
        // 去掉参数view_position
        String trimmedPosition = processedCommand.substring(13);
        trimmedPosition.trim();

        // 将参数转换为整数
        int position = trimmedPosition.toInt();

        // 打印指定位置的衣物情况
        if (position > 0 && position < 6) {
            String message = "Position " + String(position) + ": ";
            if (positions[position].isEmpty) {
                message += "Empty";
            } else {
                message += "Clothing: ";
                message += positions[position].cloth->output();
            }
            dataManager->logData(message, true);
        } else if (position == 0) {
            dataManager->logData("Default position", true);
        } else {
            dataManager->logData("Invalid position index", true);
        }
    } else {
        // 不继续向下处理
        dataManager->logData("Unknown command in ShelfManager: " + processedCommand, true);
        return false;
    }
    return true;
}

