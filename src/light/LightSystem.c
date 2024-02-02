#include <pins_arduino.h>
#include <Arduino.h>
#include <USBAPI.h>
#include <wiring_private.h>
#include <HardwareSerial.h>

void adjustLight(lightIntensity, potentiometerValue);

// 定义引脚和变量
int lightSensorPin = A0;    // 光敏电阻连接的模拟输入引脚
int potentiometerPin = A1;  // 可变电阻连接的模拟输入引脚
int ledPin = 3;             // LED连接的数字输出引脚
int lightSensorValue = 0;   // 存储从光敏电阻读取的值
int potentiometerValue = 0; // 存储从可变电阻读取的值
int light = 0;              // 定义灯的开度，范围0-100

// 设置函数
void setup() {
    Serial.begin(9600);       // 开始串行通信，设置波特率为9600
    pinMode(ledPin, OUTPUT);  // 设置LED引脚为输出模式
}

// 主循环函数
void loop() {
    // 从光敏电阻读取值
    lightSensorValue = analogRead(lightSensorPin);  // 读取光敏电阻的值（范围0~1023）
    // 映射光照强度到0-100范围
    int lightIntensity = map(lightSensorValue, 0, 1023, 0, 100);

    // 读取可变电阻的值（范围0~1023），并映射到PWM输出范围0-255
    potentiometerValue = map(analogRead(potentiometerPin), 0, 1023, 0, 255);

    // 调整灯光
    adjustLight(lightIntensity, potentiometerValue);

    // 打印传感器读数到串行监视器
    Serial.print("Light Sensor Value: ");
    Serial.print(lightIntensity);
    Serial.print("%\tPotentiometer Value: ");
    Serial.print(map(potentiometerValue, 0, 255, 0, 100));
    Serial.print("%\tlightIntensity: ");
    Serial.print(light);
    Serial.println(light > 0 ? "(on)" : "(off)");

    delay(1000);  // 暂停1秒
}

/*
调整光线算法规则：
如果衣柜关闭，即感受到的光照强度小于50%,那么认为衣柜没有打开门。衣柜不开启辅助灯光。此时用户可调整的范围为30%
如果衣柜开启，即感受到的光照强度大于50%,开启辅助灯光到70%。默认灯光强度70%。
衣柜开启后，根据可调电阻和光照情况调整光线。用户能够调整的范围为70%。
如果可调电阻值为0，则不开启灯光
*/

// 根据光照强度和可变电阻值调整灯光
void adjustLight(int lightIntensity, int potValue) {
    if (potValue == 0) {  // 如果可变电阻值为0
        light = 0;          // 不开启灯光
    } else if (lightIntensity < 65) {  // 如果衣柜关闭（光照强度<65%）
        // 衣柜关闭时，灯光不开启，用户可调整范围为30%
        light = map(potValue, 0, 255, 0, 30);
    } else {
        // 衣柜开启（光照强度>=65%），默认灯光强度70%
        light = 70;
        // 衣柜开启后，根据可调电阻调整光线，用户可调整范围为70%
        light = max(light, map(potValue, 0, 255, 0, 70));
    }

    analogWrite(ledPin, map(light, 0, 100, 0, 255));  // 调整LED亮度
}

