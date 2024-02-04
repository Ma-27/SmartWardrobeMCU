//
// Created by Mamh on 2024/2/3.
//

#include "hardware_abstraction/HardwareAbstraction.h"
#include "hardware_abstraction/sensors/SensorManager.h"  // 确保包含SensorManager的头文件
#include "utility/ProjectConfig.h"
#include "data/DataManager.h"

// 初始化静态实例指针
HardwareAbstraction *HardwareAbstraction::instance = nullptr;

// 提供一个公共的访问方法
HardwareAbstraction *HardwareAbstraction::getInstance() {
    if (instance == nullptr) {
        instance = new HardwareAbstraction();
    }
    return instance;
}

// 初始化所有硬件组件，这是该类的默认构造函数
HardwareAbstraction::HardwareAbstraction() {
    // 获取显示组件的实例
    displayManager = DisplayManager::getInstance();
    // 获取传感器组件的实例
    sensorManager = SensorManager::getInstance();
}


// 采集温湿度并且显示在LCD屏幕上
void HardwareAbstraction::processTemperatureAndHumidity(boolean enabled) {
    if (!enabled) return;

    float humidity = sensorManager->readHumidity();
    float temperature = sensorManager->readTemperature();

    displayManager->displayHumidity(humidity);
    displayManager->displayTemperature(temperature);
}

// 其他管理器的接口方法实现，如传感器数据读取和执行器控制

