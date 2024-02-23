/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/23 下午 08:45
 */

#include <Arduino.h>
#include "utility/ProjectConfig.h"

class Light {
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

    // 设置灯光强度
    void setLightIntensity(int intensity);
};
