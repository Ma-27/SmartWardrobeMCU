## 命令管理与执行详解

命令管理器(`CommandManager`)扮演着智能衣柜系统中极为关键的角色，它负责解析用户输入的命令并据此执行对应的动作，以实现设备控制等功能。该管理器涵盖了从灯光控制到摄像头操作的多种设备控制命令。

### 命令格式规范

命令遵循一定的格式规范`<操作对象>  (<操作对象> ...) -<命令> [-<参数>]`，例如，`act light -on`告知执行器模块的灯光打开。

### 详细命令列表与解释

命令详解如下，分为执行器和传感器两大类，以覆盖系统的各项功能需求。

### 执行器类命令：

基础命令`act`是用于指示智能衣柜执行特定动作的关键词，如操控灯光或激活除湿器等。

#### 灯光控制（执行元件 `-light`）

- **打开灯光命令：`act light -on`**
    - **作用**：此命令通过将灯光亮度设置为最大值（通常认为是100%）来打开灯光，确保衣柜内部光线充足。
    - **实现细节**：`Light::getInstance()->setLightIntensity(100);`调用`Light`类的单例实例，使用`setLightIntensity`
      方法并传入参数`100`来实现灯光调至最亮。

- **关闭灯光命令：`act light -off`**
    - **作用**：该命令通过将灯光亮度设置为最低值来关闭灯光，以节约能源或在不需要时关闭灯光。
    - **实现细节**：`Light::getInstance()->setLightIntensity(0);`同样调用`Light`类的单例实例，并传入参数`0`
      至`setLightIntensity`方法以关闭灯光。

- **自动调节灯光：`act light -auto`**
  - **作用**：该命令通过传感器检测环境亮度，自动调节灯光亮度，以保持衣柜内部的适宜亮度。
  - **实现细节**：

- **手动设置亮度值：`act light -v 72`
  - **作用**：该命令通过手动设置灯光亮度值来调节灯光亮度，允许用户根据实际需求调整灯光亮度。
  - **实现细节**：

#### 摄像头控制（执行原件 `cam`）

- **拍照命令：`sens cam -shot`**
    - **作用**：触发OV7670 FIFO摄像头模块进行一次拍照，用于捕获并存储当前衣柜内部的图像，方便用户远程查看。
    - **实现细节**：`Camera::getInstance()->captureImage();`通过调用`Camera`
      类的单例实例来执行拍照动作。但是拍摄的照片存储在AL422芯片中，此命令通过串口打印图片数据，或者立刻传回数据。

- **上传图像命令：`sens cam -upload`** （待实现）
    - **作用**：将OV7670 FIFO摄像头模块最近一次拍摄的图像上传至指定服务器或云平台，允许用户远程访问和查看衣柜内部的情况。
    - **实现细节**：该动作需要结合网络通信模块实现图像数据的发送，具体实现取决于所选云平台的API和网络协议要求。

#### 网络（执行原件 `net`）

- **上传原始数据：`net upload -raw "data"`**（待实现）
    - **作用**：向云平台直接上传数据。数据不经过打包，直接上传。
    - **实现细节**：

### 命令架构图

以下是命令层级树的ASCII画图表示：

```
CommandManager
|
|-- act
|   |-- light
|   |   |-- -on
|   |   |-- -off
|   |   |-- -auto
|   |   |-- -v <int>
|
|-- sens
|   |-- cam
|       |-- -shot
|       |-- -upload
|
|-- net
|    |-- -tx
|      |-- -raw "data"
|       
```

这个ASCII树结构清晰地展示了智能衣柜系统中的命令层级关系。`CommandManager`作为顶层管理器，下分为`act`（执行器类命令）、`sens`
（传感器控制命令）和`net`（网络数据传输命令）三大主命令类别。每个主类别下又细分为具体的设备或操作对象，如`light`和`cam`
，以及它们对应的操作命令，例如`-on`、`-off`、`-shot`和`-upload`。特别地，`net -tx -raw "data"`命令直接传输原始数据至云平台。

这种结构化的命令设计，旨在提供清晰的操作逻辑和高效的命令执行路径，以便用户可以直观地与系统进行交互。

通过上述命令管理与执行的详细解释，用户能够明确每个命令的具体作用及其背后的实现逻辑，从而更高效地与智能衣柜系统进行交互。