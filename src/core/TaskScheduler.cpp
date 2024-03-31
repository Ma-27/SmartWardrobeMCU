/**
 * @description: TaskScheduler类定义，用于定时执行任务。
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/12 下午 07:08
 */

#include "core/TaskScheduler.h"

// 静态成员变量定义，用于存储TaskScheduler类的唯一实例的指针。
TaskScheduler *TaskScheduler::instance = nullptr;

// 获取TaskScheduler类的唯一实例的引用。
// 如果实例不存在，将创建一个新的实例并返回其引用。
TaskScheduler &TaskScheduler::getInstance() {
    if (instance == nullptr) {
        // 如果单例未被创建，创建之，并初始化最大任务数量为20。
        static TaskScheduler singleton(20);
        instance = &singleton;
    }
    return *instance;
}

// TaskScheduler的构造函数。
// 初始化任务容器和任务数量。
TaskScheduler::TaskScheduler(unsigned int capacity) : capacity(capacity), taskCount(0) {
    // 分配足够容纳所有任务的空间。
    tasks = new Task[capacity];
}

// TaskScheduler的析构函数。
// 释放分配给任务的内存。
TaskScheduler::~TaskScheduler() {
    // 清理分配的任务数组。
    delete[] tasks;
}

/** 添加一个新任务到调度器。
 @param callback: 要执行的任务的回调函数。
 @param interval: 任务执行的时间间隔（毫秒）。
 @return taskId: 这个任务的ID。
 */
int TaskScheduler::addTask(TaskCallback callback, unsigned long interval) {
    // 确保当前任务数量没有超出容量。
    if (taskCount < capacity) {
        // 为新任务设置回调函数、间隔和最后运行时间。
        tasks[taskCount].id = generateUniqueId(); // 假设这个函数能生成一个唯一的ID
        tasks[taskCount].callback = callback;
        tasks[taskCount].interval = interval;
        tasks[taskCount].lastRun = millis(); // 使用Arduino的millis()函数获取当前时间。
        taskCount++; // 增加已注册任务的数量。
    }

    // 注意返回值。因为taskCount已经+1.
    return tasks[taskCount - 1].id;
}

/** 删除一个已注册的任务。
 @param callback: 要删除的任务的回调函数。
 注意：如果有多个任务使用相同的回调，此方法将删除第一个匹配的任务。
*/
int TaskScheduler::deleteTask(int id) {
    for (unsigned int i = 0; i < taskCount; i++) {
        if (tasks[i].id == id) {
            // 找到要删除的任务，从数组中移除它。
            // 这是通过将后面的任务前移来实现的，覆盖掉要删除的任务。
            for (unsigned int j = i; j < taskCount - 1; j++) {
                tasks[j] = tasks[j + 1];
            }
            taskCount--; // 减少任务的总数。

            // 可选：将最后一个任务位置清零，确保不留下悬挂指针或数据。
            // 注意：这一步不是必须的，因为taskCount已经减少，
            // 并且未来可能会有新任务覆盖此位置。
            tasks[taskCount].callback = nullptr;
            tasks[taskCount].interval = 0;
            tasks[taskCount].lastRun = 0;

            // 由于已经找到并删除了任务，退出循环。
            return 0;
        }
    }

    // 代表未找到任务，删除失败。
    return -1;
}

// 执行所有已注册的任务，如果它们的间隔时间已经到了。
void TaskScheduler::run() {
    // 获取当前机器已经运行的时间。
    unsigned long currentMillis = millis();
    for (unsigned int i = 0; i < taskCount; i++) {
        // 检查是否到达任务执行的时间。
        if (currentMillis - tasks[i].lastRun >= tasks[i].interval) {
            tasks[i].callback(); // 执行任务的回调函数。
            tasks[i].lastRun = currentMillis; // 更新任务的最后运行时间。
        }
    }
}

// 生成唯一标识符的函数
int TaskScheduler::generateUniqueId() {
    // 增加ID值并返回
    return ++lastId;
}


