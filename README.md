# 物联网智能衣柜设计-Arduino硬件部分源代码

使用Arduino due开发板，基于C++语言，实现智能衣柜的硬件部分。ESP8266模块作为网络通信模块，DHT11传感器作为温湿度传感器，步进电机作为衣架控制模块，LED灯作为灯光控制模块。

## 架构

架构不仅包括MVC的元素，还融合了模块化的思想。

### 设计架构/接口与数据流

1. **核心控制单元** (`CoreController`)
    - 负责整体的业务逻辑控制。
    - 协调传感器读取、命令解析、状态管理和网络通信。
    - 接口与数据流
        - 接口：提供初始化 (`init`), 更新 (`updateTemperatureAndHumidity`), 和命令处理 (`handleCommand`) 方法。
        - 数据流：从`SensorManager`接收数据，向`ActuatorManager`发送指令，通过`NetworkManager`与云平台通信。
        - 串口：打印数据流等等。
2. **传感器管理器** (`SensorManager`)
    - 负责管理所有传感器，如DHT11。
    - 提供统一的接口供核心控制单元查询环境数据。
    - 接口与数据流
        - 接口：提供读取 (`read`) 和初始化 (`init`) 方法。
        - 数据流：定期向核心控制单元提供传感器数据。
3. **执行器管理器** (`ActuatorManager`)
   - 负责管理所有执行器，如LED灯、步进电机等等可以被控制驱动的电子单元。
    - 根据核心控制单元的指令执行动作。
    - 接口与数据流
        - 接口：提供动作执行 (`executeAction`) 方法。
        - 数据流：接收核心控制单元的动作指令并执行。
4. **网络管理器** (`NetworkManager`)
    - 负责处理所有网络通信。
    - 接收来自云平台的指令，并将数据发送至云平台。
    - 接口与数据流
        - 接口：提供数据发送 (`sendData`) 和接收命令 (`receiveCommand`) 方法。
        - 数据流：双向通信，接收指令和发送数据。
5. **显示管理器** (`DisplayManager`)
    - 负责管理与显示相关的所有操作，如LED屏幕显示。
    - 显示来自核心控制单元的状态和信息。
    - 接口与数据流
        - 接口：提供显示 (`display`) 方法。
        - 数据流：从核心控制单元接收要显示的信息。
6. **设备抽象层** (`HardwareAbstraction`)
    - 为传感器和执行器、显示器提供统一的抽象接口。
    - 使得在不同硬件之间切换时不需要修改核心逻辑。
    - 接口与数据流
        - 接口：定义硬件操作的通用接口。
        - 数据流：为核心控制单元提供设备状态和执行操作的能力。
7. **数据管理器** (`DataManager`)
   - 中心化管理和调度数据流。负责数据的存储和校验，协调数据的在各个组件之间的流通等等。
   - 结合串口管理器 (SerialManager)，利用串口通信进行调试信息的输出和读取。
   - 接口与数据流
      - 接口：提供数据处理 (processData), 数据存储 (storeData), 数据发送 (sendData) 以及接收命令 (receiveCommand) 等方法。
      - 数据流：建立起复杂的数据交互路径。它不仅接收和发送数据，还负责将调试信息输出到串口，以及读取串口数据进行分析或调试。

### 文件结构

```
/SmartWardrobe
|-- /src
|   |-- main.ino
|   |-- /core
|   |   |-- CoreController.h
|   |   |-- CoreController.cpp
|   |-- /hardware_abstraction
|   |   |-- HardwareAbstraction.h
|   |   |-- HardwareAbstraction.cpp
|   |   |-- /sensors
|   |   |   |-- SensorManager.h        
|   |   |   |-- SensorManager.cpp
|   |   |-- /display
|   |   |   |-- DisplayManager.h       // LCD显示的实现
|   |   |   |-- DisplayManager.cpp
|   |   |-- /actuators
|   |   |   |-- ActuatorManager.h       // LED执行器的实现
|   |   |   |-- ActuatorManager.cpp
|   |-- /network
|   |   |-- NetworkManager.h
|   |   |-- NetworkManager.cpp
|   |-- /data
|   |   |-- DataManager.h
|   |   |-- DataManager.cpp
|   |   |-- SerialManager.h
|   |   |-- SerialManager.cpp
|   |-- /utility
|   |   |-- Utility.h
|   |   |-- Utility.cpp
|-- README.md


```

系统架构设计图：

```
                             +------------------+
                             |  CoreController  |
                             +------------------+
                                      |
     +--------------------------------+-------------------------------------+---------------------------+
     |                                |                                     |                           |
     |                      +----------------+                      +----------------+             +----------------+
     |                      |                |                      |                |             |                |
     |                      | Hardware       |                      | Network        |             | Data           |
     |                      | Abstraction    |                      | Manager        |             | Manager        |
     |                      +----------------+                      +----------------+             +----------------+
     |                              |                                       |                              |
     |            +-----------------+------------------+                    v                              v
     |            |                 |                  |            +----------------+             +----------------+
     |            v                 v                  v            |                |             |                |
     |  +----------------+  +----------------+  +----------------+  | External       |             | Local or       |
     |  | Sensors        |  | Display        |  | Actuators      |  | Systems        |             | External       |
     |  +----------------+  +----------------+  +----------------+  | (e.g., Cloud   |             | Systems data   |
     |                                                              |   Platform)    |             | serial tracing |
     |                                                              | management     |             | (e.g., PC,     |
     |                                                              +----------------+             |  other Arduino)|
     |                                                                                             +----------------+
     |
     +----------------+
     |                |
     | Utility        |
     +----------------+
```

`CoreController`通过`HardwareAbstraction`层与各个硬件模块进行交互，而`HardwareAbstraction`层进一步细分为`sensors`、`display`和`actuators`子模块，每个子模块负责特定类型硬件的抽象和实现。这种分层和模块化的设计使得系统更易于扩展和维护，同时也方便针对特定硬件类别的优化。 此架构允许系统的每个部分独立工作，并通过定义好的接口与其他部分通信。这样的设计有助于在不同层之间保持清晰的边界，易于测试和维护。此外，如果将来需要更换传感器或执行器硬件，只需更新相应的硬件抽象层代码，而不会影响到核心控制逻辑。

## Arduino的接线

2 - DHT11 DATA
18 TX1-RX1 ESP8266
19 RX1-TX1 ESP8266
20 SDA-SDA LED
21 SCL-SCL LED
// 光敏电阻连接的模拟输入引脚
static const int lightSensorPin = A0;

// 可变电阻连接的模拟输入引脚
static const int potentiometerPin = A1;

// LED连接的数字输出引脚
static const int lightPin = DAC0;

// 设置步进电机引脚，总步数，每步的步长（以微秒为单位）
Unistep2 stepper(23, 25, 27, 29, 4096, 4096);

23/25/27/29 步进电机 IN 1 2 3 4

### 串口打印调试信息的方法

//FIXME
DataManager::getInstance()->sendData("this is the data ", true);

测试
https://docs.platformio.org/en/latest/advanced/unit-testing/index.html

## TODO 日志系统

为Arduino Due设计一个日志系统，您可以创建一个简单的日志库，用于记录和输出调试信息。这个库可以提供基本的日志功能，如输出错误、警告和信息日志到串口或者SD卡。您可以定义不同的日志级别，并在运行时根据需要启用或禁用特定级别的日志。

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



