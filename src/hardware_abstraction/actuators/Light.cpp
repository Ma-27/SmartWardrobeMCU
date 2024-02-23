/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/23 下午 08:45
 */

#include "Light.h"
#include "utility/ProjectConfig.h"

// 初始化静态实例指针
Light *Light::instance = nullptr;

// 提供一个公共的访问方法
Light *Light::getInstance() {
    if (instance == nullptr) {
        instance = new Light();
        instance->initLight();
    }
    return instance;
}

// 初始化所有执行器组件，这是该类的默认构造函数
Light::Light() {
    // KEEP IT EMPTY
}

void Light::initLight() {
    // 设置DAC输出的分辨率为12位
    analogWriteResolution(12);
}

void Light::setLightIntensity(int intensity) {
    // 将0-100范围的lightValue映射到0-4095
    int mappedValue = map(intensity, 0, 100, 0, 4095);

    // lightPin是DAC引脚（例如A0），使用analogWrite直接写入映射后的值
    analogWrite(ProjectConfig::lightPin, mappedValue);
}
