/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/10 下午 12:09
 */

#include "data/cloth/ClothManager.h"

// 构造函数
ClothManager::ClothManager(int _capacity) {
    capacity = _capacity;
    clothes = new Cloth[capacity];
    count = 0;
}

// 析构函数
ClothManager::~ClothManager() {
    delete[] clothes;
}

// 添加衣物
void ClothManager::addCloth(Cloth newCloth) {
    if (count < capacity) {
        clothes[count++] = newCloth;
    }
}

// 获取衣物
Cloth ClothManager::getCloth(int index) {
    if (index >= 0 && index < count) {
        return clothes[index];
    }
    // 创建一个默认的Cloth对象，为每个参数指定默认值或空值
    return {
            "", // id: 空字符串
            "", // color: 空字符串
            "", // style: 空字符串
            "", // material: 空字符串
            "", // size: 空字符串
            false, // isInCloset: false，特定值
            -1,  // hangPosition: -1，特定值
            "", // brand: 空字符串
            "", // purchaseDate: 空字符串
            true, // isClean: true，默认值
            ""  // lastWornDate: 空字符串
    };
}

// 移除衣物
bool ClothManager::removeCloth(int index) {
    if (index < 0 || index >= count) return false;
    for (int i = index; i < count - 1; i++) {
        clothes[i] = clothes[i + 1];
    }
    count--;
    return true;
}

// 清除所有衣物
void ClothManager::clearAll() {
    count = 0;
}