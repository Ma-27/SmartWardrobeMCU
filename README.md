# 物联网智能衣柜设计-Arduino硬件部分源代码

使用Arduino Due开发板，基于C++语言，实现智能衣柜的硬件部分。ESP8266模块作为网络通信模块，DHT11传感器作为温湿度传感器，步进电机（衣架控制模块）和LED灯作为灯光控制模块。

## 架构

架构不仅包括MVC的元素，还融合了模块化的思想。

### 设计架构/接口与数据流

1. **核心控制单元** (`CoreController`)
   - 负责整体的业务逻辑控制。
   - 协调传感器读取、命令解析、数据管理和网络通信。
   - 接口与数据流（它和与它相关的类）
     - 接口：提供初始化 (`init`), 连接到网络 (`connectToWifi`), 和命令处理 (`handleCommand`) 方法。
     - 数据流：从`SensorManager`接收数据，向`ActuatorManager`发送指令，通过`NetworkManager`与云平台通信。
     - 串口：打印数据流等等。
2. **传感器管理器** (`SensorManager`)
   - 负责管理所有传感器，如摄像头、光感应器、电位器感应器以及DHT11。
   - 提供统一的接口供核心控制单元查询环境数据。
   - 接口与数据流
     - 接口：提供读取 (`read`) 和初始化 (`init`) 方法。
     - 数据流：定期向核心控制单元提供传感器数据。
3. **执行器管理器** (`ActuatorManager`)
   - 负责管理所有执行器，如加热器、冷却器、加湿器、除湿器、灯光控制和衣架控制。
   - 根据核心控制单元的指令执行动作。
   - 接口与数据流
     - 接口：提供动作执行等方法，还有一些典型控制函数，比如使用 `PID` 算法控制温度的相关执行器的函数 `autoControlTemperature` 和`autoControlHumidity` 等等，用来保持环境参数稳定在一个区间内部。
     - 数据流：接收核心控制单元的动作指令并执行。
4. **网络管理器** (`NetworkManager`)
   - 负责处理所有网络通信。
   - 接收来自云平台的指令，并将数据发送至云平台。
   - 接口与数据流
     - 接口：提供数据发送 (`sendData`) 和接收数据 (`receiveData`) 方法。此外，和它相关的类进行数据序列化或者反序列化、数据包排队解析等操作。
     - 数据流：双向通信，接收数据和发送数据。
5. **显示管理器** (`DisplayManager`)
   - 负责管理与显示相关的所有操作，如LCD显示。
   - 显示来自核心控制单元的状态和信息，显示温湿度数据和进行的各种操作信息。
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
   - 结合串口管理器 (`SerialManager`)，利用串口通信进行调试信息的输出和读取。
   - 接口与数据流
     - 接口：提供数据处理 (`processData`), 数据存储 (`storeData`), 数据发送 (`sendData`) 以及接收命令 (`receiveCommand`) 等方法。
     - 数据流：建立起复杂的数据交互路径。它不仅接收和发送数据，还负责将调试信息输出到串口，以及读取串口数据进行分析或调试。

### 代码的结构

```
/SmartWardrobe
|-- /src
|   |-- main.ino
|   |-- /core
|   |   |-- CoreController
|   |   |-- TaskScheduler
|   |   |-- CommandManager
|   |-- /hardware_abstraction
|   |   |-- HardwareAbstractions
|   |   |-- /sensors
|   |   |   |-- SensorManager     
|   |   |   |-- Camera
|   |   |   |-- LightSensors
|   |   |   |-- PotentiometerSensor
|   |   |   |-- TemperatureHumiditySensor
|   |   |-- /display
|   |   |   |-- DisplayManager   
|   |   |   |-- LCDManager              // LCD显示的实现
|   |   |-- /actuators
|   |   |   |-- ActuatorManager
|   |   |   |-- Heater
|   |   |   |-- Cooler
|   |   |   |-- Humidifier
|   |   |   |-- DeHumidifier
|   |   |   |-- Light
|   |   |   |-- ShelfManager            // 衣架控制类
|   |-- /network
|   |   |-- NetworkManager
|   |   |-- NetworkDataHandler          // 处理接收到的数据
|   |   |-- ServerConnector             // 负责连接到云平台和断开连接
|   |   |-- /net_message
|   |   |   |-- PacketGenerator         // 生成Json格式的数据包
|   |   |   |-- PacketParser            // 反序列化数据包
|   |   |   |-- PacketQueue
|   |-- /data
|   |   |-- DataManager
|   |   |-- SerialManager
|   |   |-- /pub-sub
|   |   |   |-- EventManager
|   |   |   |-- Message
|   |   |   |-- Subscriber
|   |   |-- /cloth
|   |   |   |-- Cloth
|   |   |   |-- ClothManager
|   |-- /utility
|   |   |-- Utility
|-- README.md
```

系统架构设计图：

```
                                                +------------------------+
                                                |          Core          |
                                                |  (dispatch / command)  |
                                                +------------------------+
                                                             |
     +------------------------------+------------------------+-------------+-------------------------------+
     |                              |                                      |                               |
     |                      +----------------+                      +----------------+             +----------------+
     |                      |                |                      |                |             |                |
     |                      | Hardware       |                      | Network        |             | Data           |
     |                      | Abstraction    |                      | Manager        |             | Manager        |
     |                      +----------------+                      +----------------+             +----------------+
     |                              |                                       |                              |
     |            +-----------------+------------------+                    v                              v
     |            |                 |                  |            +----------------+             +----------------+
     |            v                 v                  v            |                |             |                |
     |  +----------------+  +----------------+  +----------------+  | Cloud Plantform|             | Local or       |
     |  | Sensors        |  | Display        |  | Actuators      |  |     OneNet     |             | External       |
     |  +----------------+  +----------------+  +----------------+  |    ^       |   |             |                |
     |                                                              |    |       V   |             | serial tracing |
     |                                                              |  data   command|             | (e.g., PC )    |
     |                                                              |                |             +----------------+
     |                                                              +----------------+                             
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

## 通信报文格式设计

[报文格式与设计](doc/protocol.md)

