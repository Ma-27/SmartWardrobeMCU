## TODO 日志系统

创建一个简单的日志库，用于记录和输出调试信息。这个库可以提供基本的日志功能，如输出错误、警告和信息日志到串口或者SD卡。您可以定义不同的日志级别，并在运行时根据需要启用或禁用特定级别的日志。

### 日志系统设计思路：

1. **日志级别**：定义不同的日志级别（如DEBUG、INFO、WARNING、ERROR）。
2. **输出目标**：选择日志输出的目标，如串口（Serial Monitor）或SD卡。
3. **时间戳**：为每条日志添加时间戳，以便追踪事件发生的时间。
4. **格式化**：提供格式化日志消息的功能，使日志条目易于阅读和分析。

### 示例代码：

```cpp
// LogSystem.h
#ifndef
LOGSYSTEM_H
#define
LOGSYSTEM_H

#include
"Arduino.h"

enum LogLevel {
DEBUG,
INFO,
WARNING,
ERROR
};

class LogSystem {
public:
static void begin(long baudRate);
static void log(LogLevel level, const String &message);
private:
static LogLevel currentLogLevel;
};

#endif
```

```cpp
// LogSystem.cpp
#include
"LogSystem.h"

LogLevel LogSystem::currentLogLevel = DEBUG; // 默认日志级别

void LogSystem::begin(long baudRate) {
Serial.begin(baudRate);
while (!Serial); // 等待串口连接
}

void LogSystem::log(LogLevel level, const String &message) {
if (level < currentLogLevel) return; // 忽略低于当前设置级别的日志

String logPrefix;
switch (level) {
case DEBUG: logPrefix = "DEBUG: "; break;
case INFO: logPrefix = "INFO: "; break;
case WARNING: logPrefix = "WARNING: "; break;
case ERROR: logPrefix = "ERROR: "; break;
}
Serial.println(logPrefix + message);
}
```

使用此系统，您可以轻松地在代码中添加日志语句来跟踪程序的执行流程和状态。例如：

```cpp
LogSystem::begin(9600);
LogSystem::log(DEBUG, "程序启动");
LogSystem::log(INFO, "正在执行操作...");
LogSystem::log(WARNING, "内存使用率高");
LogSystem::log(ERROR, "操作失败！");
```

根据需要调整日志级别和输出目标，以适配您的具体应用场景。