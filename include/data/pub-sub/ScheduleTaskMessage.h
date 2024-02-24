/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/22 下午 04:55
 */

#ifndef SCHEDULE_TASK_MESSAGE_H
#define SCHEDULE_TASK_MESSAGE_H

#include "data/pub-sub/Message.h"

// 任务类型枚举
enum class TaskType {
    // 一次性任务
    OneTime = 0,

    // 周期性任务
    Periodic = 1,
};

// 标识EventManager的可用情况和状态
enum class EventManagerStatus {
    // 还不行
    NOT_AVAILABLE = 0,

    // 可以使用了
    AVAILABLE = 1,
};


class ScheduleTaskMessage : public Message {
private:
    // 状态用一个枚举类的对象表示.
    EventManagerStatus status = EventManagerStatus::NOT_AVAILABLE;

public:
    ScheduleTaskMessage(EventManagerStatus status) : status(status) {

    }


    EventManagerStatus getStatus() const {
        return status;
    }
};

#endif // SCHEDULE_TASK_MESSAGE_H

