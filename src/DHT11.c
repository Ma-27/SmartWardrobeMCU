#include <LiquidCrystal_PCF8574.h>  // 包含用于I²C LCD的库
#include <HardwareSerial.h>
#include "DHT.h"                    // 包含用于DHT温湿度传感器的库

#define DHTPIN 2                   // 定义连接DHT传感器的引脚为2号
#define DHTTYPE DHT11               // 定义使用的DHT传感器类型为DHT11

// #define DHTTYPE DHT22            // 如果使用的是DHT22传感器，则取消该行注释
// #define DHTTYPE DHT21            // 如果使用的是DHT21传感器，则取消该行注释
LiquidCrystal_PCF8574 lcd(

0x27);    // 初始化LCD对象，设置I²C地址为0x27

DHT dht(

DHTPIN, DHTTYPE);           // 初始化DHT对象

void setup() {
    Serial.begin(9600);               // 初始化串行通信，波特率为9600
    Serial.println("Smart Clothing System Start");    // 在串口监视器上打印消息
    Serial.println("DHT11 test");     // 在串口监视器上打印消息

    dht.begin();                      // 初始化DHT传感器

    lcd.begin(16, 2);                 // 初始化LCD为16列2行
    lcd.setBacklight(255);            // 设置背光亮度
    lcd.clear();                      // 清除LCD显示
    lcd.setCursor(0, 0);              // 设置LCD光标位置为第一行第一列
    lcd.print("Smart Closet");        // 在LCD上显示文本
    lcd.setCursor(0, 1);              // 设置LCD光标位置为第二行第一列
    lcd.print("hello user");          // 在LCD上显示文本
} // setup()

void loop() {
    float humidity = dht.readHumidity();        // 读取湿度值
    float temperature = dht.readTemperature();  // 读取温度值（摄氏度）

    // 检查读取的温度和湿度值是否有效
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;  // 如果数据无效，则不进行以下操作
    }

    // 在串口监视器上显示湿度和温度
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");

    // LCD设置
    lcd.clear();                       // 清除LCD显示
    lcd.setCursor(0, 0);               // 设置LCD光标位置为第一行第一列
    lcd.print("Humidity:");            // 在LCD上显示“Humidity:”
    lcd.setCursor(9, 0);
    lcd.print(humidity);               // 显示湿度值
    lcd.setCursor(15, 0);
    lcd.print("%");                    // 显示百分号

    lcd.setCursor(0, 1);               // 设置LCD光标位置为第二行第一列
    lcd.print("Temperat:");             // 在LCD上显示“Tempera:”
    lcd.setCursor(9, 1);
    lcd.print(temperature);            // 显示温度值
    lcd.setCursor(14, 1);
    lcd.print((char) 223);              // 显示摄氏度符号
    lcd.setCursor(15, 1);
    lcd.print("C");                    // 显示“C”

    delay(5000);                       // 等待5秒
}
