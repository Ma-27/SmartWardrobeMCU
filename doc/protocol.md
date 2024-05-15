## 通信协议设计

注意在调试之前，要先发送报文鉴权信息：

```json
*627051#KimirrCloset#test*
```

测试库和依赖函数的可用性的脚本：

```json
*627051#KimirrCloset#testlib*
```

==禁止在报文key or value字段中使用 "，" 或者 "," 符号，因为它们被我用作了提取解析报文字段的符号。==

#### 基础数据格式

上传和下发的报文采用 `JSON` 格式编码。Arduino设备使用 `Arduino JSON`  解析库解析报文，而云平台使用自己搭建的脚本解析报文并且做一些运算和数据分发处理。

以下是一个最基础的报文元素示例。全部报文都含有下面的三个字段，不管是上行还是下发。

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Example"
}
```

以下是一些字段的设计和作用：

`device_id`是设备的唯一标识符，这通常是唯一的，因为我只搭建了一台设备，所以编号为 `001`。服务器的`device_id`编号固定为 `0`。

字段 `from` 标记数据是由谁发送的，这个的值对应于 `device_id` 的标识符。当Arduino 发送数据报文时，报文中这两个字段通常是一致的。

`packet_type`是报文的类型。服务器和 Arduino 收到对方发来的报文之后，首先根据这个字段来判断报文的类型，然后再进行后续的处理。

云平台的 `JSON` 解析允许嵌套到最多两层，并且计划在未来支持数组解析。其他键值对解析均支持。

其他一些特色字段解释如下：

`runtime` 与 `send_count `字段为了标记同一个设备报文的先后次序。注意：Arduino 无法外部授时，所以`runtime` 仅仅标记 Arduino 开机以来经历的时间；只有Arduino发送的报文具有 `runtime` 标记，而服务器端是通过`send_count` 来标记报文顺序的。每发送一次报文， `send_count ` 字段自动增加1。


------

### Arduino -> 云平台（上行）

#### 1. Ping -Pong

用来测试服务器发送和接受数据、功能选择和鉴别、Arduino系统网络响应、报文解析、命令分发是不是正常的。

Ping -Pong 测试会在`data` 字段中原样返回 Arduino 发送给服务器的数据，所以它也被用作环回测试。

Arduino会向服务器发送以下报文格式：

```json
{
  "device_id": 1,
  "from": 1,
  "packet_type": "Ping",
  "runtime": 405669,
  "data": "This is data from Arduino - testing server"
}
```

注意 Arduino 发送给云平台的命令是`Ping` ，用来标记是客户端递交给服务器的。

设备应该接收到 `Pong` 报文，即 `packet_type` 为 `Pong` 。如果 `Pong` 报文校验通过，代表两方的网络数据处理等等功能是无问题的。

#### 2. 上传温湿度传感器数据

Arduino 每30秒向服务器上报一次温湿度数据。报文格式如下：

```json
{
  "device_id": 1,
  "from": 1,
  "packet_type": "Temperature-Humidity",
  "data": {
    "temperature": 22.74,
    "humidity": 38.12,
    "measure_time": 1627875660
  },
  "runtime": 405669
}
```

此外，服务器也可以命令 Arduino上传温湿度数据，见下文。

==在未来（现在还未实现）==，温湿度数据考虑支持批量上报，以减少网络请求的频率和提高效率。这要求服务器能够解析和处理批量数据：

```json
{
  "device_id": 1,
  "from": 1,
  "packet_type": "Temperature-Humidity",
  "runtime": 405669,
  "data": [
    {
      "temperature": 22.74,
      "humidity": 38.12,
      "measure_time": 1627875660
    },
    {
      "temperature": 23.00,
      "humidity": 37.80,
      "measure_time": 1627875660
    }
  ]
}
```

#### 3. 设备登录

允许设备在首次上线时自动登录到服务器，并接收必要的配置信息。

Arduino会向服务器发送以下报文格式：

```json
{
  "device_id": 1,
  "from": 1,
  "packet_type": "Log-In",
  "runtime": 405669,
  "user_name": "Micheal",
  "password_hash": "82af731bca2cd35ea479f3f63be70a39t0f0p682"
}
```

服务器应该发回响应设备注册报文，或者发送鉴权异常信息。



#### 4.  上传灯光数据

此报文旨在每30秒向服务器上报一次灯光状态数据，报文格式如下：

```json
{
  "device_id": 1,
  "from": 1,
  "packet_type": "Light",
  "data": {
    "brightness": 150,
    "knob_value": 75,
    "light_openness": 60,
    "measure_time": 1627875660
  },
  "runtime": 405669
}
```

- **device_id**: 设备标识符，用于标识发送报文的设备。
- **from**: 发送源的标识符，通常与 `device_id` 相同。
- **packet_type**: 报文类型，此处为 `"Light-Status"` 表明这是一个关于灯光状态的数据报文。
- **data**: 包含具体灯光数据的对象。
  - **brightness**: 当前亮度值，数值范围根据实际硬件和需求定义。
  - **knob_value**: 用户通过旋钮手动调节的值，表示用户对亮度的偏好或调节。
  - **light_openness**: 灯光的开度值，表示灯光开启的程度或范围。
  - **measure_time**: 数据测量时的时间戳。
- **runtime**: 自设备启动以来的运行时间（毫秒），用于追踪报文发送的相对时间。



#### 4.  上传执行器数据

此报文旨在每30秒向服务器上报一次执行器数据，比如，加热片工作状态、衣架机械臂的位置和机械臂的开闭状态。以机械臂为例，报文格式如下：

```json
{
  "device_id": 1,
  "from": 1,
  "packet_type": "Actuator",
  "data": {
    "mechine_arm_open": false,
    "mechine_arm_position": 1
  },
  "runtime": 405730
}
```

- **device_id**: 设备标识符，用于标识发送报文的设备。
- **from**: 发送源的标识符，通常与 `device_id` 相同。
- **packet_type**: 报文类型，此处为 `"Actuator"` 代表这个和执行器的部件有关。
- **data**: 包含具体数据的对象。
  - **mechine_arm_open**: 机械臂是否开启。机械臂开启指的是机械臂的位置是否处于拾取状态，还是非拾取状态（默认状态）。
  - **mechine_arm_position**: 机械臂的位置，用于追踪当前机械臂跟踪的哪个衣服。机械臂需要对相应的衣服进行操作
- **runtime**: 自设备启动以来的运行时间（毫秒），用于追踪报文发送的相对时间。


------

### 云平台 -> Arduino（下发）

#### 1. Ping -Pong

用来测试服务器功能选择和鉴别是否正常、Arduino系统网络响应、报文解析、命令分发是不是正常的。

服务器收到Arduino报文后，会回复一个 `pong` 报文：

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Pong", // 注意这里是Pong，用来标记是服务器返回给客户端的
  "send_count": 1,
  "data": "This is data from Arduino, testing server"
}
```

此时应该由Arduino处理并且解析 `JSON` 报文。 `Pong` 报文不同于默认报文。`Pong` 报文旨在测试脚本的分层处理等各种函数和功能是否正常运行了。一般不会随便发送，除非命令为 `Ping` ；而默认回复报文是在接收到数据后，就会应答的报文。

#### 2. 服务器默认回复报文

服务器接收到数据后，马上回复确认报文，格式如下：

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Confirm",
  "data": "server received data",
  "send_count": 2,
  "remark": "temperature humidity data received"
}
```

这个确认报文只是回复设备数据已经收到。

大多数响应报文都会包含一个备注字段。如果服务器成功处理了请求，它将在 `remark` 字段中提供具体的反馈信息。若处理过程中发生错误，服务器则会返回一个错误报文。在某些情况下，如果服务还在处理中且暂时无法确定命令是否成功执行，`remark` 字段可能会留空。例如，在温湿度数据的响应报文中，`remark` 通常会注明“温湿度数据已经收到”。如果温湿度数据验证成功，`remark` 将显示 “temperature humidity data received” 。反之，如果数据解析失败，`remark` 将显示 “failed to parse data” 。

其他的验证信息也会在 `remark` 字段中给出详细说明。

对于服务器会发回响应报文，如果light报文可以被正常处理，则服务器会在remark中回复：“light related data received”。



#### 3. 服务器请求数据报文

如果收到用户 App 等客户端更新数据的请求，则服务器需要向 Arduino 系统请求报文。请求 Arduino 报文的数据格式如下：

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Request-Data",
  "data_type": "Temperature-Humidity",
  "send_count": 3,
  "remark": ""
}
```

除了可以要求 `Temperature-Humidity` 这种类型之外， 还有 `Light`  等等数据类型与 `request_data` 命令对应，这个和 `data_type` 中所列出的数据类型一致。`remark` 字段视情况而定，默认为空。

Arduino 应该尽快向服务器返回符合要求的数据。返回数据的格式和Arduino -> 云平台（上行）中所列出的一致。

#### 4. 异常报文

这里是一套错误代码和相应的错误消息，用于处理通信过程中可能发生的错误。例如，当设备发送的数据格式不正确或时，应返回一个包含错误代码和描述的 `JSON`报文：

`error_code`字段有：

1. 通用错误码

- **1000**：未知错误 - 当系统发生不可预知的错误时使用。
- **1001**：参数错误 - 当输入参数不满足API规范时使用。比如，`packet_type` 中没有匹配到处理函数，无法处理报文要求的内容。
- **1002**：权限不足 - 当用户试图访问他们没有权限的资源时使用。
- **1003**：资源不存在 - 请求的资源不存在，可能文件已经被移动或者节点已经被删除。

2. 网络相关错误码

- **2001**：网络超时 - 请求超出了预定的时间。
- **2002**：连接失败 - 无法建立网络连接。
- **2003**：数据传输错误 - 数据在传输过程中发生错误。

3. 数据库相关错误码

- **3001**：数据库连接失败 - 无法连接到数据库。
- **3002**：数据库操作失败 - 执行数据库操作时发生错误。
- **3003**：数据冲突 - 数据库操作由于并发冲突失败。

4. 认证授权错误码

- **4001**：认证失败 - 用户名或密码错误。
- **4002**：Token过期 - 提供的Token已过期。
- **4003**：Token无效 - 提供的Token无效。

5. 设备相关错误码（特定于物联网或类似场景）

- **5001**：设备离线 - 请求的设备未连接到网络。
- **5002**：设备响应错误 - 设备给出了异常响应，或者是不在预期中的响应。
- **5003**：设备配置错误 - 设备因配置错误无法完成请求。

6. 报文错误

- **6001**：无效报文，比如 `JSON` 数据的括号没有正确闭合、不符合基本的`JSON`格式。
- **6002**：报文解析时发生错误。查看`error_message` 了解详情。

比如说，如果匹配 `packet_type` 时，发现没有对应的报文类型。此时，服务器不处理请求，而是直接中断处理这个任务，然后发送参数错误报文。 `error_message ` 字段中有不同的注释提示。

```json
{
  "device_id": "1",
  "from": "0",
  "packet_type": "Error",
  "send_count": "5",
  "error_code": "1001",
  "error_message": "Invalid parameter in parsing packet_type."
}
```

如果经过初步校验发现 `JSON`有异常，比如 `JSON` 数据的括号没有正确闭合、不符合基本的`JSON`格式，则发送错误信息报文：

```json
{
  "device_id": "1",
  "from": "0",
  "packet_type": "Error",
  "send_count": "5",
  "error_code": "6001",
  "error_message": "Invalid JSON format."
}
```

目前还没有错误处理机制。服务器遇到错误后，发送此类报文便中断执行，废弃掉未完成的部分。

#### 5. 设备登录确认

服务器响应设备注册，可能包含设备配置信息：

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Register-Ack",
  "config": {
    "valid_interval": 300000,
    "status": "success"
  }
}
```



#### 6. 命令报文

##### 1. 开启/关闭灯光控制命令

当服务器需要控制智能衣柜的灯光开关时，它会发送一个具体指定操作的命令报文。这个报文通过`action`字段明确指示灯光的预期状态，如`"turn_on"`或`"turn_off"`。

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Command",
  "command": "Light-Manuel",
  "action": "turn_on",  // 可替换为 "turn_off" 以关闭灯光
  "remark": "Command to turn on the light"  // 描述信息也应相应更改为 "turn off"
}
```

##### 2. 自动灯光控制命令

服务器也可以下发命令，要求Arduino设备根据环境光照或其他传感器输入自动控制灯光。这类命令通过`action`字段指示设备启用或禁用自动控制功能。

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Command",
  "command": "Auto-Light-Control",
  "action": "enable",  // 可替换为 "disable" 以禁用自动灯光控制
  "remark": "Command to enable automatic light control"
}
```

##### 报文说明

- **device_id**: 设备的唯一标识符，确保命令发送到正确的设备。
- **from**: 发送者的标识符，这里固定为 `0` 表示来自服务器。
- **packet_type**: 报文类型，这里固定为 `"Command"`，表示这是一个控制命令报文。
- **command**: 指定命令的种类，如 `"Light-Manuel"` 或 `"Auto-Light-Control"`。
- **action**: 具体的操作指令，如 `"turn_on"`、`"turn_off"`、`"enable"` 或 `"disable"`。
- **remark**: 对命令的额外描述，便于日志记录和问题追踪。



##### 3.温度控制命令

服务器下发命令，设定智能衣柜的目标温度，并且输入用户是否启用。这类命令通过`action`字段指示设备启用或禁用自动控制功能。

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Command",
  "command": "Temperature-Control",
  "action": "enable",  // 可替换为 "disable" 以禁用自动温湿度控制
  "target" : 18,  // 控制的目标温度
  "remark": "Command to enable the target temperature"
}
```

报文说明同上。

##### 4.湿度控制命令

服务器下发命令，设定智能衣柜的目标湿度，并且输入用户是否启用。这类命令通过`action`字段指示设备启用或禁用自动控制功能。

```json
{
  "device_id": 1,
  "from": 0,
  "packet_type": "Command",
  "command": "Humidity-Control",
  "action": "enable",  // 可替换为 "disable" 以禁用自动温湿度控制
  "target" : 52,  // 控制的目标温度
  "remark": "Command to set the target humidity"
}
```

报文说明同上。







------


### 附加类

附加类中的协议可能有的实现了，有的没有实现。它们是对等的，即，Arduino 和云平台使用同样的解析格式。

#### 协议版本控制

为了支持未来的扩展和兼容性，引入`version`字段来标识通信协议的版本。这样，即使协议在未来进行了更新，Arduino设备和云平台也可以兼容处理不同版本的协议。

```json
{
  "version": "1.0",
  "device_id": 1
}
```



------

