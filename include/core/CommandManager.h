/**
 * @description: 命令管理器类，用于解析和执行输入的命令
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/3/30 下午 09:06
 */

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

class DataManager;

#include "hardware_abstraction/actuators/Light.h"
#include "hardware_abstraction/sensors/Camera.h"
#include "data/DataManager.h"
#include "utility/ProjectConfig.h"
#include "CommandListener.h"

class CommandManager : public CommandListener {
private:
    static CommandManager *instance; // 静态私有实例指针

    // 私有化构造函数
    CommandManager();

    // 初始化所需要的数据，这是该类的默认构造函数
    void initCommandManager();

    // 保存数据管理器的一个实例
    DataManager *dataManager;

public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    CommandManager(const CommandManager &) = delete;

    void operator=(const CommandManager &) = delete;

    // 提供一个公共的访问方法
    static CommandManager *getInstance();

    // 解析并执行命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;
};

#endif //CommandManager.h