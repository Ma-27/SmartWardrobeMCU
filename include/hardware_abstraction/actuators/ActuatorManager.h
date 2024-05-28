/**
 * @description: 执行器管理器** (`ActuatorManager`)。这里负责管理各个能够执行具体任务的器件。这里也充当控制算法的所在地，这里负责控制执行器的自动逻辑，比如自动温湿度等等。
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:26
 */


#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include "data/pub-sub/Message.h"
#include "data/pub-sub/EventManager.h"
#include "hardware_abstraction/actuators/Light.h"
#include "data/pub-sub/Subscriber.h"
#include "core/CommandListener.h"
#include "Humidifier.h"
#include "Dehumidifier.h"
#include "Heater.h"
#include "Cooler.h"
#include "ShelfManager.h"

class SerialManager;

class DataManager;

/**
 * **执行器管理器** (`ActuatorManager`)
    - 负责管理所有执行器，如LED灯、步进电机等等可以被控制驱动的电子单元。
    - 根据核心控制单元的指令执行动作。
 */
class ActuatorManager : public Subscriber, public CommandListener {
private:
    static ActuatorManager *instance; // 静态私有实例指针

    // 私有化构造函数
    ActuatorManager();

    /**
    * 实现Subscriber接口要求的update方法。
    *
    * @param message 收到的消息
    * @param messageType 收到的消息类型，int类型号
    */
    void update(const Message &message, int messageType) override;

    void initActuatorManager();

    // 保存事件接收器的一个对象
    EventManager *eventManager;

    // 灯光控制类
    Light *light;

    // 加湿器控制类
    Humidifier *humidifier;

    // 干燥器控制类
    Dehumidifier *dehumidifier;

    // 冷却器控制类
    Cooler *cooler;

    // 加热器控制类
    Heater *heater;

    // 衣架系统
    ShelfManager *hanger;


    // 数据管理器
    DataManager *dataManager;

    // 自动控制温度,这个方法由调度器调度
    void autoControlTemperature(boolean enabled);

    // 自动控制湿度,这个方法由调度器调度
    void autoControlHumidity(boolean enabled);

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    ActuatorManager(const ActuatorManager &) = delete;

    void operator=(const ActuatorManager &) = delete;

    // 温度是否自动控制。温度根据用户指示控制开启或者关闭，也可以通过手动控制。手动控制的优先级高于自动控制。
    bool isTemperatureAutoControl = false;

    // 湿度是否自动控制。湿度根据用户指示控制开启或者关闭，也可以通过手动控制。手动控制的优先级高于自动控制。
    bool isHumidityAutoControl = false;

    // 提供一个公共的访问方法
    static ActuatorManager *getInstance();

    // 设置灯光强度
    void setLightIntensity(int intensity);

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif // ACTUATOR_MANAGER_H
