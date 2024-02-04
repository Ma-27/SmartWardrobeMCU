//
// Created by Mamh on 2024/2/3.
//

#include "data/DataManager.h"
#include "data/SerialManager.h"
#include "utility/ProjectConfig.h"

DataManager *DataManager::instance = nullptr;

//  DataManager的构造函数实现
DataManager::DataManager() {
    //初始化Serial Manager实例
    serialManager = SerialManager::getInstance();
}

DataManager *DataManager::getInstance() {
    if (instance == nullptr) {
        instance = new DataManager();
    }
    return instance;
}

void DataManager::saveData(const String &data, bool SerialPrint) {
    storedData = data; // 简单示例：存储数据
    if (SerialPrint) {
        serialManager->println(data); // 调用 SerialManager 的 println 方法打印数据
    }
    // 可以在这里添加更多处理逻辑
}

