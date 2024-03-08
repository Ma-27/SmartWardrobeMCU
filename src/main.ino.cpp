# 1 "C:\\Users\\�����\\AppData\\Local\\Temp\\tmph_ql33z4"

#include <Arduino.h>

# 1 "E:/CScodes/Bachelor_Graduation_Project/SmartWardobe(MCU-C-Source)/src/main.ino"

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

void setup();

void loop();

#line 15 "E:/CScodes/Bachelor_Graduation_Project/SmartWardobe(MCU-C-Source)/src/main.ino"

void setup() {


    pinMode(ProjectConfig::ledPin, OUTPUT);

    digitalWrite(ProjectConfig::ledPin, LOW);


    controller = CoreController::getInstance();

    controller->init();


    if (!controller) {
        controller = CoreController::getInstance();

        if (!controller) {
            DataManager::getInstance()->logData("Empty controller!", true);
        }
    }
}


void loop() {

    controller->looper();
}