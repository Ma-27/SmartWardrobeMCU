/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/10 下午 12:09
 */

#include <utility>

#include "data/cloth/ClothManager.h"
#include "data/DataManager.h"

// 初始化静态成员
ClothManager *ClothManager::instance = nullptr;

// 单例获取方法
ClothManager *ClothManager::getInstance() {
    static int initialized = 0;
    if (instance == nullptr) {
        instance = new ClothManager(ProjectConfig::SHELF_NUMS);
        initialized = 1;
    }
    // 如果类已经初始化过，则跳过默认位置
    if (initialized) {
        instance->clothes[0].setHangPosition(-2); // 将默认位置标记为特殊用途
    }
    return instance;
}

// 析构函数
ClothManager::~ClothManager() {
    delete[] clothes;
}

// 构造函数
ClothManager::ClothManager(int _capacity) {
    capacity = _capacity;
    clothes = new Cloth[capacity];
    count = 0;
    // 初始化所有位置为未使用
    for (int i = 0; i < capacity; i++) {
        clothes[i].setHangPosition(-1);
    }
}


// 添加衣物
void ClothManager::addCloth(Cloth newCloth, int position) {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();

    // 检查位置是否合法（需要跳过索引0的默认位置）
    if (position > 0 && position < capacity && clothes[position].getHangPosition() == -1) {
        // 位置合法且为空，添加衣物
        clothes[position] = newCloth;
        clothes[position].setIsInCloset(true);  // 标记衣物已在衣柜中
        count++;
    } else {
        // 位置不合法或已被占用，记录错误信息
        String errorMsg = "Error: Invalid position or position already occupied.";
        dataManager->logData(errorMsg, true);
    }
}


// 平方取中散列位置
int ClothManager::hashPosition(int position) {
    int hash = (position * position) % capacity;
    // 确保从不使用默认位置,如果hash为0，则返回1
    return hash == 0 ? 1 : hash;
}

// 获取衣物
Cloth ClothManager::getCloth(int index) {
    if (index >= 0 && index < capacity && clothes[index].getHangPosition() != -1) {
        return clothes[index];
    }
    // 返回默认的Cloth对象
    return Cloth();
}

// 移除衣物
bool ClothManager::removeCloth(int index) {
    if (index < 0 || index >= capacity || clothes[index].getHangPosition() == -1) return false;
    // 重置该位置
    clothes[index] = Cloth();
    clothes[index].setIsInCloset(false);
    count--;
    return true;
}

// 清除所有衣物
void ClothManager::clearAll() {
    // 重置每个位置，但是跳过默认位置
    for (int i = 1; i < capacity; i++) {
        clothes[i] = Cloth();
    }
    count = 0;
}

// 显示衣柜位置摘要
String ClothManager::displayClosetSummary() const {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();

    String summary = "Closet Summary:\n";
    for (int i = 1; i < capacity; i++) {
        if (clothes[i].getHangPosition() != -1) {
            summary += "Position:" + String(i) + ": " + "Occupied. ID: " + clothes[i].getId() +
                       ", Color: " + clothes[i].getColor() + ", In Closet: " +
                       (clothes[i].getIsInCloset() ? "Yes" : "No") + "\n";
        } else {
            summary += "Position " + String(i) + ": Empty\n";
        }
    }
    // 打印摘要到串口
    dataManager->logData(summary, true);

    return summary;
}