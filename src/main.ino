#include "CoreController.h"
#include "CoreControllerBuilder.h"
#include "SerialManager.h"
#include "SensorManager.h"
#include "ActuatorManager.h"
#include "NetworkManager.h"
#include "DisplayManager.h"
#include "HardwareAbstraction.h"
#include "Utility.h"
#include "ProjectConfig.h"

CoreControllerBuilder builder;
CoreController *controller;

void setup() {
    // 初始化整个控制器。控制器会将必要的组件全都初始化。
    controller = CoreController::getInstance();
}

void loop() {
    // 真正的控制循环过程在controller中反复执行。确保coreController不是nullptr
    if (controller != nullptr) {
        controller->looper();
    } else {
        // 报错
        DataManager::getInstance()->sendData("Empty controller!", true);
    }
}

