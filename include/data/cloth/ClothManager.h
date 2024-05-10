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

public:
    // 构造函数
    ClothManager(int _capacity);

    // 析构函数
    ~ClothManager();

    // 管理衣物的方法
    void addCloth(Cloth newCloth);

    Cloth getCloth(int index);

    bool removeCloth(int index);

    void clearAll();
};

#endif
