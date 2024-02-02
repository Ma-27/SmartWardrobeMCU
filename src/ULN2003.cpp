#include <Unistep2.h>
#include "Unistep2.h"

// 设置步进电机引脚，总步数，每步的步长（以微秒为单位）
Unistep2 stepper(23, 25, 27, 29, 4096, 4096);

void setup() {

}

void loop() {
    stepper.run();  // 步进电机启动

    if (stepper.stepsToGo() == 0) { // 如果stepsToGo=0，表示步进电机已完成所需的步数
        delay(500);
        stepper.move(2024);    // 正转一圈
        stepper.move(-1012);  // 负数表示反转，反转一圈
    }

}
