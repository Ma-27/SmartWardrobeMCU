/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/12 下午 07:08
 */

#ifndef TaskScheduler_h
#define TaskScheduler_h

#include <Arduino.h>

#undef min
#undef max

#include <functional>

class TaskScheduler {
private:
    // 任务类型,默认是不可抢占式的
    enum TaskType {
        PREEMPTIVE,    // 可抢占式任务
        NON_PREEMPTIVE  // 不可抢占式任务
    };

    // 线程的状态模型
    enum TaskState {
        READY,   // 就绪
        RUNNING, // 运行中
        WAITING  // 等待
    };


public:
    // 定义任务回调函数的类型
    // 使用std::function而不是原始的函数指针
    using TaskCallback = std::function<void(void)>;
    using ResumeFunction = std::function<void(void)>;

    // 获取单例对象的引用
    static TaskScheduler &getInstance();

    // 添加任务函数（传入函数名），返回taskID。
    int addTask(TaskCallback callback, unsigned long interval, String info = "", TaskType type = NON_PREEMPTIVE,
                int priority = 0);

    // 删除任务函数（传入任务id）
    int deleteTask(int id);

    // 每次的任务调度
    void run();

    // 让出处理机，让其他任务执行
    void yield(int taskId, ResumeFunction func);

    // 夺回处理机，继续执行本任务
    void resume(int taskId);

    // 打印任务信息
    void printTaskInfo();


private:
    // 禁止复制和赋值
    TaskScheduler(const TaskScheduler &) = delete;

    TaskScheduler &operator=(const TaskScheduler &) = delete;

    // 单例模式的自身实例
    static TaskScheduler *instance;

    // 静态变量，用于存储上一个分配的ID
    int lastId = 0;

    // 记录当前正在执行的任务的索引
    unsigned int currentTaskIndex = 0;

    // 任务结构体
    struct Task {
        // 任务的标识符
        int id;
        // 回调函数（地址）
        TaskCallback callback;
        // 规定的执行的时间间隔
        unsigned long interval;
        // 上次运行的时刻
        unsigned long lastRun;
        TaskType type;
        TaskState state;
        // 优先级，如果需要优先级调度
        int priority;
        // 是否要让出处理机
        bool shouldYield = false;
        // 任务的描述信息
        String info;
        // yield后resume的回调函数
        ResumeFunction func;
    };

    Task *tasks;
    unsigned int capacity;
    unsigned int taskCount;

    // 为每个任务分配一个独有的ID。
    int generateUniqueId();

    // 私有构造函数
    TaskScheduler(unsigned int capacity);

    // 私有析构函数
    ~TaskScheduler();
};

#endif



