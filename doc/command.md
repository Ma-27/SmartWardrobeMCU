### 命令管理与执行详解

命令管理器(`CommandManager`)扮演着智能衣柜系统中极为关键的角色，它负责解析用户输入的命令并据此执行对应的动作，以实现设备控制等功能。该管理器涵盖了从灯光控制到摄像头操作的多种设备控制命令。

### 命令格式规范

命令遵循一定的格式规范`<操作对象> (<操作对象> ...) -<命令> [-<参数>]`，例如，`act light -on`告知执行器模块的灯光打开。

### 详细命令列表与解释

命令详解如下，分为执行器和传感器两大类，以覆盖系统的各项功能需求。

### 执行器类命令

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
  - **实现细节**
    ：系统通过环境光传感器检测当前亮度，并计算需要的灯光强度，调用`Light::getInstance()->setLightIntensity(intensity);`
    来调整亮度。

- **手动调节灯光：`act light -manuel`**
  - **作用**：通过 APP，用户手动控制灯光的开关。
  - **实现细节**：系统接收来自APP的手动调节指令，调用对应的`Light`类方法进行灯光控制。

- **手动设置亮度值：`act light -v 72`**
  - **作用**：该命令通过手动设置灯光亮度值来调节灯光亮度，允许用户根据实际需求调整灯光亮度。
  - **实现细节**：调用`Light::getInstance()->setLightIntensity(72);`来设定亮度。

#### 摄像头控制（执行原件 `cam`）

- **拍照命令：`sens cam -shot`**
    - **作用**：触发OV7670 FIFO摄像头模块进行一次拍照，用于捕获并存储当前衣柜内部的图像，方便用户远程查看。
    - **实现细节**：`Camera::getInstance()->captureImage();`通过调用`Camera`
      类的单例实例来执行拍照动作。拍摄的照片存储在AL422芯片中，通过串口打印图片数据，或者立刻传回数据。

- **上传图像命令：`sens cam -upload`**
    - **作用**：将OV7670 FIFO摄像头模块最近一次拍摄的图像上传至指定服务器或云平台，允许用户远程访问和查看衣柜内部的情况。
  - **实现细节**
    ：结合网络通信模块实现图像数据的发送，通过调用网络模块的上传方法，如`NetworkManager::getInstance()->uploadImage(imageData);`
    ，实现图像数据的上传。

#### 温湿度自动控制

- **开启自动控制温度：`act auto-control-temperature -tv 24`**
  - **作用**：开启自动控制温度的功能。如果自动控制已经开启了，此命令无效。后面参数指定了目标温度(此处为24度)。
  - **实现细节**
    ：调用温度传感器实时检测温度，通过控制加热器或降温设备来维持目标温度，具体实现如`TemperatureController::getInstance()->setTargetTemperature(24);`。

- **关闭自动控制温度：`act manual-control-temperature`**
  - **作用**：关闭自动控制温度的功能。如果本来就是关闭的，此命令无效。
  - **实现细节**：停止自动控制逻辑，通过调用`TemperatureController::getInstance()->disableAutoControl();`来实现。

- **开启自动控制湿度：`act auto-control-humidity -hv 32`**
  - **作用**：开启自动控制湿度的功能。如果自动控制已经开启了，此命令无效。
  - **实现细节**
    ：调用湿度传感器实时检测湿度，通过控制加湿器或干燥器来维持目标湿度，具体实现如`HumidityController::getInstance()->setTargetHumidity(32);`。

- **关闭自动控制湿度：`act manual-control-humidity`**
  - **作用**：关闭自动控制湿度的功能。如果本来就是关闭的，此命令无效。
  - **实现细节**：停止自动控制逻辑，通过调用`HumidityController::getInstance()->disableAutoControl();`来实现。

#### 加湿器控制（执行原件 `humidify`）

- **加湿：`act humidify -on`**
  - **作用**：湿度控制器开始喷雾增加湿度。如果湿度控制器已经开始喷雾了，此命令无效。
  - **实现细节**：调用`Humidifier::getInstance()->turnOn();`来启动加湿功能。

- **停止加湿：`act humidify -off`**
  - **作用**：湿度控制器停止喷雾。如果湿度控制器当前没有喷雾，此命令无效。
  - **实现细节**：调用`Humidifier::getInstance()->turnOff();`来关闭加湿功能。

#### 干燥器控制（执行原件 `dehumidify`）

- **立刻烘干：`act dehumidify -on`**
  - **作用**：湿度控制器电机开始转动，将空气送入干燥剂制成的空气通道，然后再返送到衣柜中，主要利用除湿风扇原件降低湿度。如果干燥控制器已经开始工作了，此命令无效。
  - **实现细节**：调用`Dehumidifier::getInstance()->turnOn();`来启动干燥功能。

- **结束烘干：`act dehumidify -off`**
  - **作用**：湿度控制器停止干燥。风扇停止转动，将空气送入干燥器的通道关闭。如果干燥器没有工作，则此命令无效。
  - **实现细节**：调用`Dehumidifier::getInstance()->turnOff();`来关闭干燥功能。

- **干燥风扇开度控制：`act dehumidify -v 132`**
  - **作用**：调节除湿风扇到一定的开度，手动控制。
  - **实现细节**：调用`Dehumidifier::getInstance()->setFanSpeed(132);`来调整风扇速度。

#### 降温器（执行原件 `cool`）

- **立刻降温：`act cool -on`**
  - **作用**：降温风扇开启。如果原件已经开始工作了，此命令无效。
  - **实现细节**：调用`Cooler::getInstance()->turnOn();`来启动降温功能。
  
- **关闭降温：`act cool -off`**
  - **作用**：降温风扇关闭，风扇停止转动。如果元件没有工作，则此命令无效。
  - **实现细节**：调用`Cooler::getInstance()->turnOff();`来关闭降温功能。
  
- **降温风扇开度控制：`act cool -v 178`**
  - **作用**：调节降温风扇到一定的开度，手动控制。
  - **实现细节**：调用`Cooler::getInstance()->setFanSpeed(178);`来调整风扇速度。

#### 加热器（执行原件 `heat`）

- **立刻加热：`act heat -on`**
  - **作用**：打开加热片进行加热，如果加热控制器已经开始工作了，此命令无效。
  - **实现细节**：调用`Heater::getInstance()->turnOn();

`来启动加热功能。

- **关闭加热：`act heat -off`**
  - **作用**：关闭加热片，如果加热控制器正处在关闭状态，此命令无效。
  - **实现细节**：调用`Heater::getInstance()->turnOff();`来关闭加热功能。

#### 衣架（执行原件 `shelf`）

- **舵机角度设定（仅测试用）：`act shelf -servo_angle 45`**
  - **作用**：设定舵机转到规定的角度（这里是45度）。此函数的参数可以为负值，但必须位于-360度到+360度之间。
  - **实现细节**：调用`Shelf::getInstance()->setServoAngle(45);`来设置舵机角度。

- **皮带旋转到特定角度（仅测试用）：`act shelf -stepper_step 8000`**
  - **作用**：将皮带和舵机（模拟机械臂）旋转8000步（参数任意），8000步代表从衣架的这头到那头。此函数的参数可以为负值。
  - **实现细节**：调用`Shelf::getInstance()->moveStepper(8000);`来控制皮带旋转。

- **指定衣架位置上的衣物信息（仅测试用）：`act shelf -view_position 1`**
  - **作用**：打印某个位置上衣物情况，包括颜色、款式、这个衣架是否为空。
  - **实现细节**：调用`Shelf::getInstance()->viewPosition(1);`来获取指定位置的衣物信息。

- **衣架位置（仅测试用）：`act shelf -arm_position`**
  - **作用**：获取步进电机库中的位置。直接打印在控制台上。
  - **实现细节**：调用`Shelf::getInstance()->getArmPosition();`来获取当前机械臂的位置。

- **衣架摘要：`act shelf -summary_shelves`**
  - **作用**：获取衣架上所有挂位的信息。如果衣架上有衣物，则打印出它的摘要信息。如果衣架上没有衣物，则显示空。
  - **实现细节**：调用`Shelf::getInstance()->summaryShelves();`来获取衣架的总体信息。

- **机械臂移动到衣架的具体位置：`act shelf -moveto 0`** （BUG)
  - **作用**：直接根据位置0-5，移动到衣架的具体位置上去。后面的参数是具体的位置参数。
  - **实现细节**：调用`Shelf::getInstance()->moveToPosition(0);`来移动到指定位置。

- **挂起指定的衣物：`act shelf -add_cloth 1`**
  - **作用**：直接根据命令的参数，移动到位置0-5中的任意位置。挂起衣物并且返回到默认位置。后面的参数是具体的位置参数。
  - **实现细节**：调用`Shelf::getInstance()->addCloth(1);`来挂起指定衣物。

- **取下指定的衣物：`act shelf -pick_cloth 1`**
  - **作用**：直接根据命令的参数，移动到位置0-5中的任意位置。取下衣物并且返回到默认位置。后面的参数是具体的位置参数。
  - **实现细节**：调用`Shelf::getInstance()->pickCloth(1);`来取下指定衣物。

### 网络（执行原件 `net`）

- **上传原始数据：`net -uploadRaw "data"`**
  - **作用**：向云平台直接上传数据。数据不经过打包，直接上传。注意:data 那个字符串必须要有双引号。否则无法解析正确。
  - **实现细节**：调用`NetworkManager::getInstance()->uploadRawData("data");`来上传原始数据。

- **Ping命令：`net -ping`**
  - **作用**：检查网络连接和服务器数据处理是否正常。
  - **实现细节**：调用`NetworkManager::getInstance()->pingServer();`来检查网络连接。

- **上传温湿度数据命令：`net -dht`**
  - **作用**：立刻上传当前温湿度数据到服务器。
  - **实现细节**：调用`NetworkManager::getInstance()->uploadDHTData();`来上传温湿度数据。

- **上传光照数据命令：`net -light`**
  - **作用**：立刻上传当前光照数据到服务器。
  - **实现细节**：调用`NetworkManager::getInstance()->uploadLightData();`来上传光照数据。

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
|   |-- humidify
|   |   |-- -on
|   |   |-- -off
|   |-- dehumidify
|   |   |-- -on
|   |   |-- -off
|   |   |-- -v <int>
|   |-- cool
|   |   |-- -on
|   |   |-- -off
|   |   |-- -v <int>
|   |-- heat
|   |   |-- -on
|   |   |-- -off
|   |-- shelf
|   |   |-- -servo_angle <int>
|   |   |-- -stepper_step <int>
|   |   |-- -view_position <int>
|   |   |-- -arm_position
|   |   |-- -summary_shelves
|   |   |-- -moveto <int>
|   |   |-- -add_cloth <int>
|   |   |-- -pick_cloth <int>
|
|-- sens
|   |-- cam
|       |-- -shot
|       |-- -upload
|
|-- net
    |-- -uploadRaw "data"
    |-- -ping
    |-- -dht
    |-- -light
```

这个ASCII树结构清晰地展示了智能衣柜系统中的命令层级关系。`CommandManager`作为顶层管理器，下分为`act`（执行器类命令）、`sens`
（传感器控制命令）和`net`（网络数据传输命令）三大主命令类别。每个主类别下又细分为具体的设备或操作对象，如`light`和`cam`
，以及它们对应的操作命令，例如`-on`、`-off`、`-shot`和`-upload`。特别地，`net -uploadRaw "data"`命令直接传输原始数据至云平台。

这种结构化的命令设计，旨在提供清晰的操作逻辑和高效的命令执行路径，以便用户可以直观地与系统进行交互。