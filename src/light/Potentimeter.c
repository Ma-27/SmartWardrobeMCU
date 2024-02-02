#include <Arduino.h>

// 定义引脚和变量
int sensorPin = A1;     // 定义模拟输入引脚，连接到传感器
int ledPin = 3;         // 定义数字输出引脚，用于控制LED
int sensorValue = 0;    // 存储从传感器读取的值

// 设置函数
void setup() {
    pinMode(ledPin, OUTPUT);  // 设置LED引脚为输出模式
}

// 主循环函数
void loop() {
    // 从模拟传感器读取值
    sensorValue = analogRead(sensorPin);          // 读取模拟输入引脚的值（范围0~1023）

    // 将读取的值映射到PWM输出范围
    sensorValue = map(sensorValue, 0, 1023, 0, 255);  // 将读取的值映射到0~255范围

    // 控制LED亮度
    analogWrite(ledPin, sensorValue);  // 以PWM方式控制LED亮度
}
