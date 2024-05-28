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
float Kp_temp = 10.0; // 比例系数
float Ki_temp = 0.00025; // 积分系数
float Kd_temp = 0.1; // 微分系数

// PID 控制变量（温度）
float integralTemp = 0.0;
float lastErrorTemp = 0.0;
unsigned long lastTimeTemp = 0;

// PID 控制器参数（湿度）
float Kp_hum = 11.5; // 比例系数
float Ki_hum = 0.00023; // 积分系数
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
            TaskScheduler::getInstance().addTask(
                    [this]() { this->autoControlTemperature(ActuatorManager::isTemperatureAutoControl); },
                    ProjectConfig::SMALL_INTERVAL, "auto control temperature");
            TaskScheduler::getInstance().addTask(
                    [this]() { this->autoControlHumidity(ActuatorManager::isHumidityAutoControl); },
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
    }
        /// 以下命令在这层处理

        // 如果是自动控制温度的指令
    else if (trimmedCommand.startsWith("auto-control-temperature")) {
        // 去掉命令开头的"auto-control-temperature"，以保留"-v"的格式
        String secondaryTrimmedCommand = command.substring(24);
        secondaryTrimmedCommand.trim();

        if (secondaryTrimmedCommand.length() > 0) {
            // 如果命令不为空（即含有子层级的命令-v ），递交给其子类处理
            return dispatchCommand(secondaryTrimmedCommand, "", this);
        }
    }
        // 如果是手动控制温度的指令
    else if (trimmedCommand.startsWith("manual-control-temperature")) {
        ActuatorManager::isTemperatureAutoControl = false;
        // 关闭加热器和冷却器
        heater->turnOff();
        cooler->speedControl(0);
    }
        // 如果是自动控制湿度的指令
    else if (trimmedCommand.startsWith("auto-control-humidity")) {
        // 去掉命令开头的"auto-control-humidity"，以保留"-v"的格式
        String secondaryTrimmedCommand = command.substring(21);
        secondaryTrimmedCommand.trim();

        if (secondaryTrimmedCommand.length() > 0) {
            // 如果命令不为空（即含有子层级的命令-v ），递交给其子类处理
            return dispatchCommand(secondaryTrimmedCommand, "", this);
        }
    }
        // 如果是手动控制湿度的指令
    else if (trimmedCommand.startsWith("manual-control-humidity")) {
        isHumidityAutoControl = false;
        // 关闭加湿器和降湿器
        humidifier->turnOff();
        dehumidifier->speedControl(0);

        ///  以下命令向下层派发
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
    } else {
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
        // 去掉命令开头的"-"
        String processedCommand = command.substring(1); // 去除开头的"-"字符

        // 开启自动控制温度,处理参数
        if (processedCommand.startsWith("tv")) {
            isTemperatureAutoControl = true;

            // 如果这是temperature的值,则设置目标温度

            // 去掉参数tv
            String temperatureTempString = processedCommand.substring(2);
            temperatureTempString.trim();

            // 将参数转换为浮点数
            float pendingTemperature = temperatureTempString.toFloat();
            // 检查参数合法性
            if (pendingTemperature < 0.001f || pendingTemperature > 100.0f) {
                dataManager->logData("Invalid target temperature value:" + temperatureTempString, true);
                return false;
            }
            // 设置目标温度
            dataManager->targetTemperature = pendingTemperature;
            dataManager->logData("Target temperature set to " + String(pendingTemperature) + " In Actuator Manager",
                                 true);
        }
            // 开启自动控制湿度,处理参数
        else if (processedCommand.startsWith("hv")) {
            // 解析目标湿度
            isHumidityAutoControl = true;

            // 如果这是humidity的值,则设置目标湿度

            // 去掉参数hv
            String humidityTempString = processedCommand.substring(2);
            humidityTempString.trim();

            // 将参数转换为浮点数
            float pendingHumidity = humidityTempString.toFloat();
            // 检查参数合法性
            if (pendingHumidity < 0.001f || pendingHumidity > 100.0f) {
                dataManager->logData("Invalid target humidity value:" + humidityTempString, true);
                return false;
            }
            // 设置目标湿度
            dataManager->targetHumidity = pendingHumidity;
            dataManager->logData("Target humidity set to " + String(pendingHumidity), true);
        }

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
        dataManager->logData("Temperature control disabled", false);
        return; // 如果控制不启用，则直接返回
    }

    // 读取目标温度和当前温度
    float targetTemperature = dataManager->targetTemperature;
    float currentTemperature = dataManager->temperature;
    dataManager->logData(
            "Target Temperature: " + String(targetTemperature) + ", Current Temperature: " + String(currentTemperature),
            false);

    // 获取当前时间
    unsigned long now = millis();
    // 计算时间变化
    float timeChange = (float) (now - lastTimeTemp);

    // 检查是否达到控制更新间隔
    if (timeChange >= ProjectConfig::CONTROL_DHT_TIME) {
        dataManager->logData("Time since last update: " + String(timeChange), false);

        // 计算温度偏差
        float error = targetTemperature - currentTemperature;
        // 计算积分项
        integralTemp += error * timeChange;
        // 计算微分项
        float derivative = (error - lastErrorTemp) / timeChange;
        // 计算 PID 输出
        float output = Kp_temp * error + Ki_temp * integralTemp + Kd_temp * derivative;
        // 输出限制到 PWM 范围 (-255 ~ 255)
        output = constrain(output, -255, 255);

        dataManager->logData("Error: " + String(error) + ", Integral: " + String(integralTemp) + ", Derivative: " +
                             String(derivative) + ", Output: " + String(output), false);

        // 更新历史数据
        lastErrorTemp = error;
        lastTimeTemp = now;

        // 控制加热器和冷却器的PWM。
        // 输出正值需要加热，输出负值需要冷却，输出为0则关闭加热器和冷却器。如果到达阈值，则不控制了。
        if (output > 0.1f) {
            // 打开加热器
            heater->turnOn();
            dataManager->logData("Heater turned on", false);
            // 设置冷却器的PWM值,关闭冷却器
            cooler->speedControl(0);
            dataManager->logData("Cooler turned off", false);
        } else if (output < -0.1f) {
            // 关闭加热器
            heater->turnOff();
            dataManager->logData("Heater turned off", false);
            // 设置冷却器的PWM值，输出取反
            cooler->speedControl((int) -output);
            dataManager->logData("Cooler speed set to " + String((int) -output), false);
        } else {
            dataManager->logData("No action taken", false);
        }
    }
}

// 自动控制湿度
void ActuatorManager::autoControlHumidity(boolean enabled) {
    if (!enabled) {
        dataManager->logData("Humidity control disabled", false);
        return; // 如果控制不启用，则直接返回
    }

    // 读取目标湿度和当前湿度
    float targetHumidity = dataManager->targetHumidity;
    float currentHumidity = dataManager->humidity;
    dataManager->logData(
            "Target Humidity: " + String(targetHumidity) + ", Current Humidity: " + String(currentHumidity), false);

    // 获取当前时间
    unsigned long now = millis();
    // 计算时间变化
    float timeChange = (float) (now - lastTimeHum);

    // 检查是否达到控制更新间隔
    if (timeChange >= ProjectConfig::CONTROL_DHT_TIME) {
        dataManager->logData("Time since last update: " + String(timeChange), false);

        // 计算湿度偏差
        float error = targetHumidity - currentHumidity;
        // 计算积分项
        integralHum += error * timeChange;
        // 计算微分项
        float derivative = (error - lastErrorHum) / timeChange;
        // 计算 PID 输出
        float output = Kp_hum * error + Ki_hum * integralHum + Kd_hum * derivative;
        // 输出限制到 PWM 范围 (-255 ~ 255)
        output = constrain(output, -255, 255);

        dataManager->logData("Error: " + String(error) + ", Integral: " + String(integralHum) + ", Derivative: " +
                             String(derivative) + ", Output: " + String(output), false);

        // 更新历史数据
        lastErrorHum = error;
        lastTimeHum = now;

        // 控制加湿器和降湿器的PWM。
        // 输出正值需要加湿，输出负值需要降湿烘干。如果控制值已经非常接近了，则不控制了避免超调。
        if (output > 0.1f) {
            // 打开加湿器
            humidifier->turnOn();
            dataManager->logData("Humidifier turned on", false);
            // 设置PWM值,关闭降湿器
            dehumidifier->speedControl(0);
            dataManager->logData("Dehumidifier turned off", false);
        } else if (output < -0.1f) {
            // 关闭加湿器
            humidifier->turnOff();
            dataManager->logData("Humidifier turned off", false);
            // 设置降湿器的PWM值，输出取反
            dehumidifier->speedControl((int) -output);
            dataManager->logData("Dehumidifier speed set to " + String((int) -output), false);
        } else {
            dataManager->logData("No action taken", false);
        }
    }
}

