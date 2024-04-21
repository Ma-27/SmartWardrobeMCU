/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/23 下午 08:45
 */

#include "hardware_abstraction/actuators/Light.h"
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

// 解析命令。TODO 注意这是最底层的命令解析了。这里只考虑了处理参数和执行命令，没有考虑命令的传递。
bool Light::parseCommand(const String &command) {
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
bool Light::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 删除命令前的所有空格
    command.trim();

    // 去掉命令开头的"-"，以保留"-on"的格式
    String processedCommand = command.substring(1);
    processedCommand.trim();


    //  执行命令并且处理参数
    if (processedCommand.startsWith("on")) {
        // 设置灯光强度全开,并且由命令控制
        isAutoControl = false;
        setLightIntensity(100);
    } else if (processedCommand.startsWith("off")) {
        // 设置灯光强度全关,并且由命令控制
        isAutoControl = false;
        setLightIntensity(0);
    } else if (processedCommand.startsWith("auto")) {
        // 自动调节亮度
        isAutoControl = true;
    } else if (processedCommand.startsWith("manuel")) {
        // 自动调节亮度
        isAutoControl = false;
    } else if (processedCommand.startsWith("v")) {
        // 根据命令调节亮度
        isAutoControl = false;
        // 去掉参数v
        String intensityString = processedCommand.substring(1);
        intensityString.trim();

        // 将参数转换为整数
        int intensity = intensityString.toInt();
        // 检查参数合法性
        if (intensity < 0 || intensity > 100) {
            Serial.println("Invalid intensity value:" + intensityString);
            return false;
        }
        // 设置灯光强度
        setLightIntensity(intensity);

    } else {
        // 不继续向下处理
        Serial.println("Unknown command in Light: " + processedCommand);
        return false;
    }
    return false;
}

