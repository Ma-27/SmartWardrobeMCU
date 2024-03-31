/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/23 下午 08:45
 */

#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>
#include "utility/ProjectConfig.h"
#include "core/CommandListener.h"

class Light : public CommandListener {
private:
    static Light *instance; // 静态私有实例指针

    // 私有化构造函数
    Light();

    // 初始化灯光的真正函数
    void initLight();

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    Light(const Light &) = delete;

    void operator=(const Light &) = delete;

    // 提供一个公共的访问方法
    static Light *getInstance();

    // 灯光是否自动控制。灯光可以根据亮度和光敏电阻的值自动调节，但也可以被控制开启或者关闭。手动控制的优先级高于自动控制。
    bool isAutoControl = true;

    // 设置灯光强度
    void setLightIntensity(int intensity);

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // LIGHT_H