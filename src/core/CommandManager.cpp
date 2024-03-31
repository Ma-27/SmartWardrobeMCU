/**
 * @description: 命令管理器类，用于解析和执行输入的命令
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/3/30 下午 09:06
 */

#include "core/CommandManager.h"
#include "hardware_abstraction/sensors/SensorManager.h"

// 初始化静态实例指针
CommandManager *CommandManager::instance = nullptr;

// 提供一个公共的访问方法
CommandManager *CommandManager::getInstance() {
    if (instance == nullptr) {
        instance = new CommandManager();
        instance->initCommandManager();
    }
    return instance;
}

// 初始化所需要的数据，这是该类的默认构造函数
void CommandManager::initCommandManager() {
    // 保存数据管理器的一个实例
    dataManager = DataManager::getInstance();
}

// 私有化构造函数
CommandManager::CommandManager() {
    // KEEP IT EMPTY
}

// 解析命令
bool CommandManager::parseCommand(const String &command) {
    // 移除字符串首尾的空白字符
    String trimmedCommand = command;
    trimmedCommand.trim();

    // 解析命令并执行相应的操作
    if (trimmedCommand.startsWith("act")) {
        // 调用这个类中的parseCommand方法，对命令进行进一步解析，如果还有子层级的命令，则向下分发
        return dispatchCommand(trimmedCommand, "act", ActuatorManager::getInstance());

    } else if (trimmedCommand.startsWith("sens")) {
        // 调用这个类中的parseCommand方法，对命令进行进一步解析，如果还有子层级的命令，则向下分发
        return dispatchCommand(trimmedCommand, "sens", SensorManager::getInstance());

    } else if (trimmedCommand.startsWith("net")) {
        // 调用这个类中的parseCommand方法，对命令进行进一步解析，如果还有子层级的命令，则向下分发
        return dispatchCommand(trimmedCommand, "net", NetworkManager::getInstance());

    } else {
        // 未知命令
        dataManager->logData("Unknown command in Command Manager: " + trimmedCommand, true);
        return false;
    }
}

// 具体解析是哪个负责执行命令，派发给相应的监听器
bool CommandManager::dispatchCommand(String &command, const String &tag, CommandListener *listener) {
    // 删除命令前的所有空格
    command.trim();

    // 检查命令是否应该直接被处理（即是否以"-command"类似格式开始）
    if (command.startsWith("-")) {
        // 去掉命令开头的"-"，以保留"light -on"的格式
        String processedCommand = command.substring(1); // 去除开头的"-"字符
        // TODO 执行命令并且处理参数,这一类没什么需要处理的参数

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

    return false;
}
