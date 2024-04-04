## 通信协议设计

#### 基础数据格式

报文采用 Json 格式编码解析。Arduino使用 Json 解析库解析，而云平台使用自己搭建的脚本解析数据并且做一些必要的处理。

以下是最基本的报文部分，所有报文都应该包含这些字段。其中，`device_id`是设备的唯一标识符，这通常是唯一的，因为我只搭建了一台设备，所以编号为
001。服务器的编号为0。 `packet_type`是报文的类型。服务器和Arduino收到对方发来的报文之后，首先根据这个字段来判断报文的类型，然后再进行后续的处理。

比如说，测试用的 `ping` 报文：

```json
{
  "device_id": "1",
  "packet_type": "Ping"
}
```

服务器收到这个报文后，会回复一个 `pong` 报文：

```json
{
  "device_id": "1",
  "packet_type": ":Ping",
  "timestamp": ":405669",   // 服务器返回标准时间戳
  "send_count": "5",
  "data": "Pong"
}
```

类似这样的形式，是协议设计的基础部分。

### Arduino -> 云平台

#### 1. Ping -Pong

用来测试服务器功能选择和鉴别是否正常、Arduino系统网络响应、报文解析、命令分发是不是正常的。Arduino会向服务器发送以下报文格式：

```json
{
  "device_id": "1",
  "packet_type": "Ping"
}
```

服务器收到这个报文后，会回复一个 `pong` 报文：

```json
{
  "device_id": "1",
  "packet_type": ":Ping",
  "send_count": "1",
  // 服务器返回的发送次数
  "data": "Pong"
}
```

服务器接收

#### 2. 上传温湿度传感器数据

```json
{
  "device_id": "1",    // 通常是设备的唯一标识符，唯一设备设为001
  "packet_type": ":Temperature-Humidity-Update",  // 温湿度
  "runtime": ":405669",  // 从衣柜开机到现在的运行时间，单位ms
  "data": {
    "key1": "value1",
    "key2": "value2"
  }
}
```

### 云平台 -> Arduino

#### 1. Ping -Pong

用来测试服务器功能选择和鉴别是否正常、Arduino系统网络响应、报文解析、命令分发是不是正常的。

服务器收到Arduino报文后，会回复一个 `pong` 报文：

```json
{
  "device_id": "1",
  "packet_type": ":Ping",
  "send_count": "2",
  // 服务器返回的发送次数
  "data": "Pong"
}
```

此时应该由Arduino处理并且解析Json报文。

#### 2. 服务器默认回复报文

服务器接收到数据后，马上回复确认报文，格式如下

```json
{
  "device_id": "1",
  "packet_type": "confirm",
  "data": "server received data",
  "send_count": "3"
}
```

这个确认报文只是回复设备数据已经收到。但是如果真正解析了需要的报文，服务器会回复具体的报文确认内容。如果回复内容是server
received data，则仅仅代表服务器收到了数据，但是没有做针对性的处理。