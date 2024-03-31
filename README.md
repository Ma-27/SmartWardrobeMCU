# 物联网智能衣柜设计-Arduino硬件部分源代码

使用Arduino Due开发板，基于C++语言，实现智能衣柜的硬件部分。ESP8266模块作为网络通信模块，DHT11传感器作为温湿度传感器，步进电机作为衣架控制模块，LED灯作为灯光控制模块。

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

`CoreController`通过`HardwareAbstraction`层与各个硬件模块进行交互，而`HardwareAbstraction`
层进一步细分为`sensors`、`display`和`actuators`子模块，每个子模块负责特定类型硬件的抽象和实现。这种分层和模块化的设计使得系统更易于扩展和维护，同时也方便针对特定硬件类别的优化。
此架构允许系统的每个部分独立工作，并通过定义好的接口与其他部分通信。这样的设计有助于在不同层之间保持清晰的边界，易于测试和维护。此外，如果将来需要更换传感器或执行器硬件，只需更新相应的硬件抽象层代码，而不会影响到核心控制逻辑。

## 各个原件及功能介绍

[使用的原件和介绍](doc/hardware.md)

## Arduino的接线

[查看接线](doc/wire.md)

## 测试和命令

[查看命令如何调用的](doc/command.md)

关键API

### 1. 保存串口信息到日志系统，选择是否打印

```c
//FIXME
DataManager::getInstance()->logData("this is the data ", true);
```

### 2. 发布事件和更新事件

首先建立消息类，它是Message类的子类；然后如第三行那样发送消息

```c
// 发布状态更新消息
XXXXXXXMessage message(msg);
eventManager->notify(MESSAGE_TYPE, message);
```

[日志系统（未完成）](doc/logger.md)

