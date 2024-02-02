### 系统的硬件代码

采用MVC架构涉及将应用程序划分为三个主要组成部分：模型（Model）、视图（View）和控制器（Controller）。在Arduino中，视图通过LED屏幕显示。以下是项目的目录结构：

```
/SmartWardrobe
|-- /src
|   |-- main.ino
|   |-- /model
|   |   |-- EnvironmentData.h
|   |   |-- EnvironmentData.cpp
|   |   |-- MotorControl.h
|   |   |-- MotorControl.cpp
|   |-- /view
|   |   |-- DisplayInterface.h
|   |   |-- DisplayInterface.cpp
|   |   |-- NetworkInterface.h
|   |   |-- NetworkInterface.cpp
|   |-- /controller
|   |   |-- SystemController.h
|   |   |-- SystemController.cpp
|   |-- /utility
|   |   |-- Common.h
|   |   |-- Common.cpp
|   |-- /network
|   |   |-- NetworkManager.h
|   |   |-- NetworkManager.cpp
|   |-- /hardware
|   |   |-- SensorManager.h
|   |   |-- SensorManager.cpp
|   |   |-- LightManager.h
|   |   |-- LightManager.cpp
|   |   |-- MotorManager.h
|   |   |-- MotorManager.cpp
|-- README.md
```

在这个目录结构中：

- `main.ino`: 这是程序的入口点，它负责初始化系统并在`setup()`和`loop()`函数中调用其他模块。
- `/model`: 包含所有数据模型的文件，比如`EnvironmentData`负责存储环境数据，`MotorControl`负责管理步进电机的状态。
- `/view`: 包含所有视图相关的文件，比如`DisplayInterface`负责管理LED显示屏，`NetworkInterface`负责格式化数据以通过网络发送。
- `/controller`: 包含控制器文件，如`SystemController`，它负责协调模型和视图，并处理来自网络的命令。
- `/utility`: 包含各种辅助功能和共享资源。
- `/network`: 包含处理网络通信的文件。
- `/hardware`: 包含直接与硬件交互的文件，如传感器和光管理。
- `/lib`: 包含第三方库或自定义组件的文件夹，如DHT库和ESP8266库。

每个`.cpp`和`.h`文件将包含特定于它们职责的类和方法。例如，`SensorManager`将管理与温湿度传感器的通信，`MotorManager`将控制步进电机，而`LightManager`将负责控制LED屏幕和可能的灯光系统。

`main.ino` 的概念性框架可能如下：

```cpp
#include "SystemController.h"

SystemController systemController;

void setup() {
  systemController.init();
}

void loop() {
  systemController.update();
}
```

在此架构中，`SystemController`类将负责管理其他所有类的实例，并确保它们在适当的时候进行通信和数据更新。例如，它可能需要从`EnvironmentData`获取最新的环境读数，并通过`NetworkInterface`将其发送到云平台。

这样的结构使得代码清晰且模块化，便于单独测试每个组件，并且能够轻松地添加或修改组件而不影响其他部分。在编写具体的代码之前，建立这样的框架将有助于保持项目的组织性，并确保每个部分都有明

### Arduino的接线

// 设置步进电机引脚，总步数，每步的步长（以微秒为单位）
Unistep2 stepper(23, 25, 27, 29, 4096, 4096);

23/25/27/29 步进电机 IN 1 2 3 4


