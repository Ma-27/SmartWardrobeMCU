/**
 * @description: TaskScheduler类定义，用于定时执行任务。
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/12 下午 07:08
 */

#include "core/TaskScheduler.h"
#include "data/DataManager.h"

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
int TaskScheduler::addTask(TaskCallback callback, unsigned long interval, String info, TaskType type, int priority) {
    // 确保当前任务数量没有超出容量。
    if (taskCount < capacity) {
        // 为新任务设置回调函数、间隔和最后运行时间。
        tasks[taskCount].id = generateUniqueId(); // 假设这个函数能生成一个唯一的ID
        tasks[taskCount].callback = callback;
        tasks[taskCount].interval = interval;
        tasks[taskCount].lastRun = millis(); // 使用Arduino的millis()函数获取当前时间。
        tasks[taskCount].type = type;
        tasks[taskCount].state = READY;  // 新任务默认为就绪状态
        tasks[taskCount].priority = priority;
        tasks[taskCount].info = info; // 存储额外的信息
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
            // 在删除任务前，清理可能的额外资源
            tasks[i].callback = nullptr;  // 清空回调函数指针
            tasks[i].func = nullptr;      // 清空续接函数指针

            // 找到要删除的任务，从数组中移除它
            for (unsigned int j = i; j < taskCount - 1; j++) {
                tasks[j] = tasks[j + 1];
            }
            taskCount--; // 减少任务的总数

            // 可选：清零最后一个任务位置，避免悬挂指针
            tasks[taskCount].callback = nullptr;
            tasks[taskCount].func = nullptr;
            tasks[taskCount].info = "";
            tasks[taskCount].interval = 0;
            tasks[taskCount].lastRun = 0;
            tasks[taskCount].shouldYield = false;
            tasks[taskCount].type = NON_PREEMPTIVE;
            tasks[taskCount].state = READY;
            tasks[taskCount].priority = 0;

            // 由于已经找到并删除了任务，退出循环。
            return 0;  // 返回0表示成功删除任务
        }
    }

    // 代表未找到任务，删除失败。
    return -1;  // 返回-1表示任务未找到
}

// 执行所有已注册的任务，如果它们的间隔时间已经到了。
void TaskScheduler::run() {
    // 获取当前机器已经运行的时间。
    unsigned long currentMillis = millis();

    // 检查当前任务是否需要yield
    if (tasks[currentTaskIndex].shouldYield) {
        currentTaskIndex = (currentTaskIndex + 1) % taskCount;
    }

    for (unsigned int i = 0; i < taskCount; i++) {
        unsigned int taskIndex = (i + currentTaskIndex) % taskCount;

        // 仅当任务处于就绪状态，且达到执行时间时，才执行任务
        if (tasks[taskIndex].state == READY && currentMillis - tasks[taskIndex].lastRun >= tasks[taskIndex].interval) {
            // 如果是不可抢占式任务，设置其状态为运行中
            if (tasks[taskIndex].type == NON_PREEMPTIVE) {
                tasks[taskIndex].state = RUNNING;
            }

            // 执行任务
            tasks[taskIndex].callback();
            // 更新任务的最后运行时间。
            tasks[taskIndex].lastRun = currentMillis;

            // 不可抢占式任务执行完毕后，将状态设回就绪
            if (tasks[taskIndex].type == NON_PREEMPTIVE) {
                tasks[taskIndex].state = READY;
            }

            // 如果该任务执行后应该yield，则记录下一个任务的索引
            if (tasks[taskIndex].shouldYield) {
                currentTaskIndex = (taskIndex + 1) % taskCount;
                return;
            }
        }
    }
}

// 生成唯一标识符的函数
int TaskScheduler::generateUniqueId() {
    // 增加ID值并返回
    return ++lastId;
}

// 让出处理机，让其他任务执行
void TaskScheduler::yield(int taskId, ResumeFunction func) {
    for (unsigned int i = 0; i < taskCount; i++) {
        if (tasks[i].id == taskId) {
            tasks[i].shouldYield = true;
            tasks[i].func = func;
            return;
        }
    }
}

// 夺回处理机，并从断点后继续执行
void TaskScheduler::resume(int taskId) {
    for (unsigned int i = 0; i < taskCount; i++) {
        if (tasks[i].id == taskId && tasks[i].shouldYield) {
            tasks[i].shouldYield = false; // 取消让出处理机的标志
            tasks[i].func(); // 调用断点后的回调函数，继续执行
            return;
        }
    }
}

// TaskScheduler类中新增函数，用于打印所有任务信息
void TaskScheduler::printTaskInfo() {
    // 初始化数据管理器
    DataManager *dataManager = DataManager::getInstance();
    dataManager->logData("----------TASKS----------------", true);
    // 遍历任务列表并打印每个任务的信息
    for (unsigned int i = 0; i < taskCount; i++) {
        dataManager->logData("Task ID: " + String(tasks[i].id), true);
        dataManager->logData("Task Info: " + String(tasks[i].info), true);
        dataManager->logData("Task Yield? : " + String(tasks[i].shouldYield), true);
        dataManager->logData("Task Interval: " + String(tasks[i].interval) + " ms", true);
        dataManager->logData("Task Last Run: " + String(tasks[i].lastRun) + " ms", true);
        dataManager->logData("Task State: " + String(tasks[i].state), true);
        dataManager->logData("Task Priority: " + String(tasks[i].priority), true);
        // 打印完一个任务的信息后添加一个分隔行
        dataManager->logData("-------------------------", true);
    }
}


