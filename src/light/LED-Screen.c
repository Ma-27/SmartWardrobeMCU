#include <LiquidCrystal_PCF8574.h>
#include "../../.pio/libdeps/megaatmega2560/LiquidCrystal_PCF8574/src/LiquidCrystal_PCF8574.h"

LiquidCrystal_PCF8574 lcd(

0x27);  // 设置i2c地址，一般情况下为0x27或0x3F

void setup() {
    lcd.begin(16, 2); // 初始化LCD
    // lcd.begin(20, 4); // 如果是20x4的LCD就使用这行

    lcd.setBacklight(255);
    lcd.clear();
    lcd.setCursor(0, 0);  // 设置光标位置 (列, 行)
    lcd.print("hello world");
    lcd.setCursor(0, 1);
    lcd.print("mamh");
} // setup()

void loop() {

} // loop()
