## Arduino的接线

## 传感器和简单执行器

2 - DHT11 DATA

18 TX1-RX1 ESP8266

19 RX1-TX1 ESP8266

20 SDA-SDA LED

21 SCL-SCL LED

4 加湿器的控制线。

5 除湿风扇的控制线。

6 降温风扇的控制线

7 电热片的控制线

// 光敏电阻连接的模拟输入引脚
static const int lightSensorPin = A0;

// 可变电阻连接的模拟输入引脚
static const int potentiometerPin = A1;

// LED连接的数字输出引脚
static const int lightPin = DAC0;

// 设置步进电机引脚，总步数，每步的步长（以微秒为单位）
Unistep2 stepper(23, 25, 27, 29, 4096, 4096);

23/25/27/29 步进电机 IN 1 2 3 4

LED灯 正极DAC0 负极GND 串联55欧姆电阻

### OV7670引脚图

|       | 1       | 2          | 3            | 4        | 5    | 6    | 7    | 8    | 9     | 10     | 11     |
|-------|---------|------------|--------------|----------|------|------|------|------|-------|--------|--------|
| **1** | WRST  √ | WR(WEN)  √ | STR x        | RST    √ | D1 √ | D3 √ | D5 √ | D7 √ | VSY √ | SIOC √ | 3.3V √ |
| **2** | RRST √  | OE √       | RCK(RCLK)  √ | PWDN √   | D0 √ | D2 √ | D4 √ | D6 √ | HREF  | SIOD √ | GND √  |

OV7670引脚 - Arduino Due引脚 描述 外上内里

基础接线

3v3 3v3

gnd gnd

reset 51

pwdn gnd

SIOD SDA + 1k pullup 黑线

SIOC SCL + 1k pullup 白线

OE 50

VSYNC 3 垂直同步信号（谨慎移动，因为这个使用了中断）

HREF 9 （别看错成6了）

WRST 49 写复位

RRST 48 读复位

WEN 47 写使能

RCLK 46 读时钟

D0 44 数据线0 下

D1 45 数据线1

D2 42 数据线2 下

D3 43 数据线3

D4 40 数据线4 下

D5 41 数据线5 上

D6 38 数据线6 下

D7 39 数据线7 上
