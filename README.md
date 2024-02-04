# 物联网智能衣柜设计-Arduino硬件开发源代码

## 设计目标

设计并实现一个基于Android移动应用的智能衣柜控制系统。
基于Android移动应用的智能衣柜控制系统能实现衣柜内部环境的实时监测与控制，如湿度、温度控制等，手机应用程序能根据天气状况提供穿衣建议。这个系统还包括其他实用的小功能，例如紫外线杀菌等等。
系统将运用Arduino Mega 2560作为衣柜控制器，通过Wi-Fi与Android
App进行数据传输和控制命令的接收。除湿、自动分类、智能推荐等高级功能也将被整合进系统中，旨在为用户提供更加便捷、个性化的衣物管理体验。

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
        - 接口：提供数据发送 (`saveData`) 和接收命令 (`receiveCommand`) 方法。
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
        - 接口：提供数据处理 (processData), 数据存储 (storeData), 数据发送 (saveData) 以及接收命令 (receiveCommand) 等方法。
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

### 系统架构设计图：

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

## Arduino的接线

// 设置步进电机引脚，总步数，每步的步长（以微秒为单位）
Unistep2 stepper(23, 25, 27, 29, 4096, 4096);

23/25/27/29 步进电机 IN 1 2 3 4

DHT11-2 Digital In

ESP-01 RX-TX1 Arduino
ESP-01 TX-RX1 Arduino

Screen SDA-SDA Arduino
Screen SCL-SCL Arduino
(不是SCL1/SDA1)

### 串口打印调试信息的方法

//FIXME
DataManager::getInstance()->saveData(String(humidity) + "这里是要发送的数据" + String(temperature), true);

测试
https://docs.platformio.org/en/latest/advanced/unit-testing/index.html




