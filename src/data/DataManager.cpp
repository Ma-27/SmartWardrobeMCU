//
// Created by Mamh on 2024/2/3.
//

#include "DataManager.h"
#include "SerialManager.h"

DataManager *DataManager::instance = nullptr;

DataManager::DataManager() {
    // 构造函数实现，可能进行一些初始化操作
}

DataManager *DataManager::getInstance() {
    if (instance == nullptr) {
        instance = new DataManager();
    }
    return instance;
}

void DataManager::storeData(const String &data) {
    storedData = data; // 简单示例：存储数据
}

void DataManager::sendData(const String &data, bool SerialPrint) {
    if (SerialPrint) {
        SerialManager::getInstance()->println(data); // 调用 SerialManager 的 println 方法打印数据
    }
    // 可以在这里添加更多处理逻辑
}

