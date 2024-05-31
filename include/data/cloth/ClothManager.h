/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/10 下午 12:09
 */

// ClothManager.h
#ifndef ClothManager_h
#define ClothManager_h

#include "Cloth.h"

class ClothManager {
private:
    Cloth *clothes;   // 衣物数组
    int capacity;     // 衣物数组的容量
    int count;        // 当前衣物数量

    // 该类的唯一实例
    static ClothManager* instance;

    // 构造函数
    ClothManager(int _capacity);

    // 析构函数
    ~ClothManager();

public:
    // 禁止拷贝构造和赋值
    ClothManager(const ClothManager&) = delete;
    ClothManager& operator=(const ClothManager&) = delete;

    // 获取单例对象的方法
    static ClothManager* getInstance();

    // 管理衣物的方法
    void addCloth(Cloth newCloth, int position);

    Cloth getCloth(int index);

    bool removeCloth(int index);

    void clearAll();

    // 平方取中散列位置
    int hashPosition(int position);

    // 获取衣物的摘要信息
    String displayClosetSummary() const;

    // 根据现在的情况，找一个空位
    int findEmptyPositionUsingHash();

    // 根据衣物ID查找衣物位置
    int findClothPositionById(const String &clothId) const;
};

#endif
