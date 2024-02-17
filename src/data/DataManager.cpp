/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 下午 03:11
 */

#include <utility>

#include "data/DataManager.h"
#include "data/SerialManager.h"
#include "utility/ProjectConfig.h"

DataManager *DataManager::instance = nullptr;

//  DataManager的构造函数实现
DataManager::DataManager() {
    //初始化Serial Manager实例
    serialManager = SerialManager::getInstance();

    //初始化Event Manager实例
    eventManager = EventManager::getInstance();
}

DataManager *DataManager::getInstance() {
    if (instance == nullptr) {
        instance = new DataManager();
    }
    return instance;
}

/**
 * 将数据暂存
 * @param data
 * @param SerialPrint 决定数据是否要打印到串口
 */
void DataManager::saveData(const String &data, bool SerialPrint) {
    storedData = data; // 简单示例：存储数据
    if (SerialPrint) {
        serialManager->println(data); // 调用 SerialManager 的 println 方法打印数据
    }
    // 可以在这里添加更多处理逻辑
}

