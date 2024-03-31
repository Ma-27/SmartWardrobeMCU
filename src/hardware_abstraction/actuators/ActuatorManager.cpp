/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:26
 */

#include "hardware_abstraction/actuators/ActuatorManager.h"

// 初始化静态实例指针
ActuatorManager *ActuatorManager::instance = nullptr;

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
    // 订阅NETWORK_STATUS_CHANGE消息
    // eventManager->subscribe(FIXME LATER, this);

    // 初始化灯光控制类
    light = Light::getInstance();

    // 初始化数据管理器
    dataManager = DataManager::getInstance();
}

/**
* 实现Subscriber接口要求的update方法。
*
* @param message 收到的消息
* @param messageType 收到的消息类型，int类型号
*/
void ActuatorManager::update(const Message &message, int messageType) {

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

