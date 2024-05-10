/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:26
 */

#include "hardware_abstraction/actuators/ActuatorManager.h"
#include "hardware_abstraction/actuators/Humidifier.h"
#include "hardware_abstraction/actuators/Dehumidifier.h"
#include "hardware_abstraction/actuators/Cooler.h"
#include "hardware_abstraction/actuators/Heater.h"

// 初始化静态实例指针
ActuatorManager *ActuatorManager::instance = nullptr;

// PID 控制器参数（温度）
float Kp_temp = 2.0; // 比例系数
float Ki_temp = 0.05; // 积分系数
float Kd_temp = 0.1; // 微分系数

// PID 控制变量（温度）
float integralTemp = 0.0;
float lastErrorTemp = 0.0;
unsigned long lastTimeTemp = 0;

// PID 控制器参数（湿度）
float Kp_hum = 1.5; // 比例系数
float Ki_hum = 0.03; // 积分系数
float Kd_hum = 0.05; // 微分系数

// PID 控制变量（湿度）
float integralHum = 0.0;
float lastErrorHum = 0.0;
unsigned long lastTimeHum = 0;


// 提供一个公共的访问方法
ActuatorManager *ActuatorManager::getInstance() {
    if (instance == nullptr) {
        instance = new ActuatorManager();
        instance->initActuatorManager();
    }
    return instance;
}

// 初始化所有执行器组件，这是该类的默认构造函数
ActuatorManager::ActuatorManager() {
    // KEEP IT EMPTY
}

void ActuatorManager::initActuatorManager() {
    // 获得处理pub-sub的对象的单例
    eventManager = EventManager::getInstance();
    // 订阅调度器准备好了的消息。准备好了之后就添加各种自动控制的任务到调度器一直调度。
    eventManager->subscribe(TASK_SCHEDULER_READY, this);

    // 初始化数据管理器
    dataManager = DataManager::getInstance();

    // 初始化灯光控制类
    light = Light::getInstance();

    // 初始化加湿器
    humidifier = Humidifier::getInstance();

    // 初始化干燥器
    dehumidifier = Dehumidifier::getInstance();

    // 初始化冷却器
    cooler = Cooler::getInstance();

    // 初始化加热器
    heater = Heater::getInstance();

    // 初始化晾衣架
    hanger = ShelfManager::getInstance();
}

/**
* 实现Subscriber接口要求的update方法。
*
* @param message 收到的消息
* @param messageType 收到的消息类型，int类型号
*/
void ActuatorManager::update(const Message &message, int messageType) {
    switch (messageType) {
        case TASK_SCHEDULER_READY:
            dataManager->logData("init task scheduler ready from actuator manager", false);

            // 负责自动控制温湿度。由于温湿度控制器都在这里，所以不需要HardwareAbstraction控制。
            TaskScheduler::getInstance().addTask([this]() { this->autoControlTemperature(true); },
                                                 ProjectConfig::SMALL_INTERVAL, "auto control temperature");
            TaskScheduler::getInstance().addTask([this]() { this->autoControlHumidity(true); },
                                                 ProjectConfig::SMALL_INTERVAL, "auto control humidity");
            break;
        default:
            // DO NOTHING
            dataManager->logData("init message error(actuator manager)", true);
    }
}

// 控制灯的亮度
void ActuatorManager::setLightIntensity(int intensity) {
    light->setLightIntensity(intensity);
}


// 解析命令
bool ActuatorManager::parseCommand(const String &command) {
    // 移除字符串首尾的空白字符
    String trimmedCommand = command;
    trimmedCommand.trim();

    // 解析命令并执行相应的操作
    if (trimmedCommand.startsWith("light")) {
        // 调用这个类中的parseCommand方法，对命令进行进一步解析，如果还有子层级的命令，则向下分发
        return dispatchCommand(trimmedCommand, "light", Light::getInstance());
        // 解析命令并执行相应的操作
    } else if (trimmedCommand.startsWith("humidify")) {
        // 如果这是一个加湿器的命令
        return dispatchCommand(trimmedCommand, "humidify", Humidifier::getInstance());
    } else if (trimmedCommand.startsWith("dehumidify")) {
        // 如果这是一个降湿器的命令
        return dispatchCommand(trimmedCommand, "dehumidify", Dehumidifier::getInstance());
    } else if (trimmedCommand.startsWith("cool")) {
        // 如果这是一个降温器的命令
        return dispatchCommand(trimmedCommand, "cool", Cooler::getInstance());
    } else if (trimmedCommand.startsWith("heat")) {
        // 如果这是一个加热器的命令
        return dispatchCommand(trimmedCommand, "heat", Heater::getInstance());
    } else if (trimmedCommand.startsWith("shelf")) {
        // 如果这是一个衣架的命令
        return dispatchCommand(trimmedCommand, "shelf", ShelfManager::getInstance());
    }else {
        // 未知命令
        dataManager->logData("Unknown command in Actuator Manager: " + trimmedCommand, true);
        return false;
    }
}

// 具体解析是哪个负责执行命令，派发给相应的监听器
bool ActuatorManager::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 删除命令前的所有空格
    command.trim();

    // 检查命令是否应该直接被处理（即是否以"-command"类似格式开始）
    if (command.startsWith("-")) {
        // 去掉命令开头的"-"，以保留"light -on"的格式
        String processedCommand = command.substring(1); // 去除开头的"-"字符
        // TODO 执行命令并且处理参数

        // 不继续向下处理
        return true;
    }

    // 检查命令是否以tag开头，这一步应该去掉tag，并且使命令向下传递
    if (command.startsWith(tag)) {
        // 去掉前缀
        String tempCommand = command.substring(tag.length());
        // 删除命令前的所有空格
        tempCommand.trim();

        // 检查命令是否非空，如果还不是空的，递交给ActuatorManager处理
        if (tempCommand.length() > 0) {
            // 如果命令不为空（即含有子层级的命令），递交给其子类处理
            return listener->parseCommand(tempCommand);
        }
    }


    // 未知命令
    dataManager->logData("Unknown command in Actuator Manager: " + command, true);
    return false;
}


//----------------------------------------------------------------------------------------------------------------------------------
// 这里也充当控制算法的所在地，这里负责控制执行器的自动逻辑，比如自动温湿度等等。

// 自动控制温度函数
void ActuatorManager::autoControlTemperature(boolean enabled) {
    if (!enabled) {
        return; // 如果控制不启用，则直接返回
    }

    // 读取目标温度和当前温度
    float targetTemperature = dataManager->targetTemperature;
    float currentTemperature = dataManager->temperature;
    // 获取当前时间
    unsigned long now = millis();
    // 计算时间变化
    float timeChange = (float) (now - lastTimeTemp);

    // 检查是否达到控制更新间隔
    if (timeChange >= ProjectConfig::CONTROL_DHT_TIME) {
        // 计算温度偏差
        float error = targetTemperature - currentTemperature;
        // 计算积分项
        integralTemp += error * timeChange;
        // 计算微分项
        float derivative = (error - lastErrorTemp) / timeChange;
        // 计算 PID 输出
        float output = Kp_temp * error + Ki_temp * integralTemp + Kd_temp * derivative;
        // 输出限制到 PWM 范围 (0-255)
        output = constrain(output, 0, 255);
        // 更新历史数据
        lastErrorTemp = error;
        lastTimeTemp = now;

        // 控制加热器和冷却器的PWM。输出正值需要加热，输出负值需要冷却，输出为0则关闭加热器和冷却器。如果到达阈值，则不控制了。
        if (output > 1) {
            // 打开加热器
            heater->turnOn();
            // 设置冷却器的PWM值,关闭冷却器
            cooler->speedControl(0);
        } else if (output < 1) {
            // 关闭加热器
            heater->turnOff();
            // 设置冷却器的PWM值，输出取反
            cooler->speedControl((int) -output);
        } else {
            // DO NOTHING
        }
    }
}

// 自动控制湿度
void ActuatorManager::autoControlHumidity(boolean enabled) {
    if (!enabled) {
        return; // 如果控制不启用，则直接返回
    }

    // 读取目标湿度和当前湿度
    float targetHumidity = dataManager->targetHumidity;
    float currentHumidity = dataManager->humidity;
    // 获取当前时间
    unsigned long now = millis();
    // 计算时间变化
    float timeChange = (float) (now - lastTimeHum);

    // 检查是否达到控制更新间隔
    if (timeChange >= ProjectConfig::CONTROL_DHT_TIME) {
        // 计算湿度偏差
        float error = targetHumidity - currentHumidity;
        // 计算积分项
        integralHum += error * timeChange;
        // 计算微分项
        float derivative = (error - lastErrorHum) / timeChange;
        // 计算 PID 输出
        float output = Kp_hum * error + Ki_hum * integralHum + Kd_hum * derivative;
        // 输出限制到 PWM 范围
        output = constrain(output, 0, 255);
        // 控制除湿器的PWM速度
        dehumidifier->speedControl((int) output);
        // 更新历史数据
        lastErrorHum = error;
        lastTimeHum = now;
    }
}
