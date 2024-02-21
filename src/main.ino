#include "core/CoreController.h"
#include "core/CoreControllerBuilder.h"
#include "data/SerialManager.h"
#include "hardware_abstraction/sensors/SensorManager.h"
#include "hardware_abstraction/actuators/ActuatorManager.h"
#include "network/NetworkManager.h"
#include "hardware_abstraction/display/DisplayManager.h"
#include "hardware_abstraction/HardwareAbstraction.h"
#include "utility/Utility.h"
#include "utility/ProjectConfig.h"

CoreControllerBuilder builder;
CoreController *controller;

void setup() {
    // 先把灯灭了，省电
    // 设置数字引脚13(ledPin)为输出模式
    pinMode(ProjectConfig::ledPin, OUTPUT);
    // 将数字引脚13(ledPin)设置为低电平，熄灭LED
    digitalWrite(ProjectConfig::ledPin, LOW);

    // 初始化整个控制器。控制器会将必要的组件全都初始化。
    controller = CoreController::getInstance();
    // 控制器中，各大组件有序进行初始化工作（转控制器）
    controller->init();

    // 真正的控制循环过程在controller中反复执行。确保coreController不是nullptr
    if (!controller) {
        controller = CoreController::getInstance();
        // 再尝试初始化一遍，如果还不行就报错
        if (!controller)
            DataManager::getInstance()->saveData("Empty controller!", true);
    }
}


void loop() {
    // 真正的控制循环过程
    controller->looper();
}

