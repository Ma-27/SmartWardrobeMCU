## 各个原件及功能介绍

### OV7670 -FIFO（用于采集衣物品照片）

OV7670 FIFO版本的摄像头模块是一种广泛应用于图像处理项目的低成本CMOS摄像头模块，它配备了一个3M的FIFO（First In First
Out）内存缓冲区，用于存储摄像头捕获的图像数据，使得处理器可以以非实时的方式读取图像数据，从而降低对处理器实时性能的要求。下面是对引脚的详细解释：

| 引脚名称     | 描述                                                 |
|----------|----------------------------------------------------|
| GND      | 地（Ground）：模块的电源负极。                                 |
| 3.3V     | 电源正极：为模块提供3.3伏的电源。                                 |
| SIOD     | SCCB串行数据线（Serial Data Line）：用于OV7670与微控制器之间的数据通信。  |
| SIOC     | SCCB串行时钟线（Serial Clock Line）：控制SIOD数据线上数据传输的时钟信号。  |
| VSYNC    | 帧同步信号（Vertical Sync）：指示一帧图像的开始。                    |
| HREF     | 行同步信号（Horizontal Reference）：指示行的有效数据。              |
| PWDN     | 功耗选择模式（Power Down）：控制模块的电源关闭，通常接地或悬空。              |
| STR      | 拍照闪光控制端口（STROBE）：控制模块的电源关闭，通常接地或悬空。                |
| OE       | 输出使能（Output Enable）：用于控制FIFO数据输出的使能信号。             |
| RST      | 重置复位（Reset）：用于重置摄像头模块。                             |
| RCK/RCLK | FIFO内存读取读时钟控制端（Read Clock）：用于从FIFO读取数据的时钟信号。       |
| WR/WEN   | 写使能（Write Enable）：控制数据写入FIFO的使能信号。1有效，允许CMOS写入FIFO |
| RRST     | 读重置（Read Reset）：用于重置FIFO的读指针。                      |
| WRST     | 写重置（Write Reset）：用于重置FIFO的写指针。                     |
| D0-D7    | 数据引脚（Data Pins）：8位并行数据输出引脚，传输图像数据。                 |

### 衣柜机械臂控制

- **SERVO_DEFAULT_ANGLE (-90)**: 舵机在不操作时的默认位置，表示垂直向下的位置。
- **SERVO_HANG_ON_DEGREE (90)**: 舵机将衣物挂上的最高角度。
- **SERVO_HANG_OFF_DEGREE (45)**: 舵机取衣物时的预备角度，用于准备接近或离开衣架。
- **SERVO_MOVE_DEGREE (0)**: 舵机移动时保持此角度来存取取衣物。