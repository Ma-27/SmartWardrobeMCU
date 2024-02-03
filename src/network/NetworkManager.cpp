//
// Created by Mamh on 2024/2/2.
//

#include "NetworkManager.h"

// 初始化静态实例指针
NetworkManager *NetworkManager::instance = nullptr;

// 提供一个公共的访问方法
NetworkManager *NetworkManager::getInstance() {
    if (instance == nullptr) {
        instance = new NetworkManager();
    }
    return instance;
}

// 初始化所有硬件组件，这是该类的默认构造函数
NetworkManager::NetworkManager() {
    // TODO
}
