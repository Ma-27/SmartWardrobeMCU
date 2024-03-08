/**
 * @description: Camera class for controlling camera module operations
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/3/8 下午 04:43
 */

#include "hardware_abstraction/sensors/Camera.h"
#include "utility/ProjectConfig.h"
#include <Wire.h> // I2C 协议必不可少的库


// 初始化静态实例指针
Camera *Camera::instance = nullptr;
volatile bool Camera::interruptComing = false;
volatile int Camera::vsyncFlag = 0;


// 提供一个公共的访问方法
Camera *Camera::getInstance() {
    if (instance == nullptr) {
        instance = new Camera();
        instance->initCamera();
    }
    return instance;
}

// 这是该类的默认构造函数
Camera::Camera() {
    // KEEP IT EMPTY
}

// 这里才是真正的初始化方法。但是这里仅仅初始化必要的引脚方法等等。配置寄存器等到捕捉照片时再配置。
void Camera::initCamera() {
    interruptComing = false;
    vsyncFlag = 0;

    //IIC will start.
    Wire.begin();

    // 设置引脚模式
    ioInit();

    // 软件复位OV7670
    digitalWrite(RESET, LOW);
    delay(20);
    digitalWrite(RESET, HIGH);
}


void Camera::captureImage() {
    // Serial.println("initializing CameraISR");

    //CMOS初始化
    while (1 != initCMOS());

    // 校验读写寄存器工作是否正常
    tmp = readOv7670Reg(0x3A);
    if (tmp == 0x04) {
        // Serial.println("CMOS ID is 0x04,indicating a successful register configuration.");
    } else {
        Serial.print("ERROR IN REGISTER  CONFIGURATION,LOOPING,CMOS ID IS:");
        Serial.println(tmp);
        while (1);
    }

    // 启用中断轮询跟踪中断情况。
    attachInterrupt(digitalPinToInterrupt(VSYNC), CameraISR, FALLING);//设置触发中断的端口，中断后运行的程序和触发模式

    // process image。
    TaskScheduler &scheduler = TaskScheduler::getInstance();
    taskID = scheduler.addTask([this]() { this->checkInterruption(true); },
                               ProjectConfig::CHECK_CAMERA_INTERRUPT_DELAY);
}


void Camera::ioInit() {
    pinMode(D0, INPUT);
    pinMode(D1, INPUT);
    pinMode(D2, INPUT);
    pinMode(D3, INPUT);
    pinMode(D4, INPUT);
    pinMode(D5, INPUT);
    pinMode(D6, INPUT);
    pinMode(D7, INPUT);

    pinMode(RESET, OUTPUT);

    pinMode(FIFO_WRST, OUTPUT);
    pinMode(FIFO_WR, OUTPUT);
    pinMode(FIFO_RRST, OUTPUT);
    pinMode(FIFO_RCLK, OUTPUT);
    pinMode(FIFO_OE, OUTPUT);

    pinMode(VSYNC, INPUT_PULLUP);
}

void Camera::checkInterruption(boolean enabled) {
    if (!enabled) return;
    // 检查有没有中断事件来临
    if (interruptComing) {
        checkVsyncFlag();
    }
}

unsigned char Camera::initCMOS() {
    unsigned char resetData;

    resetData = 0x80;

    // 复位寄存器组。
    int result = writeOv7670Reg(0x12, resetData);
    if (0 == result) {
        return 0;
    }
    delay(10);

    // 设置所有的寄存器
    setRegisters();
    delay(1000);

    return 1;
}


unsigned char Camera::writeOv7670Reg(unsigned char regID, unsigned char regDat) {
    Wire.beginTransmission(OV7670_ADDRESS);
    Wire.write(regID);
    Wire.write(regDat);

    // Serial.print("Write Register: wire return value:");

    //结束通信（发送数据并且检验通信结果）
    int value = Wire.endTransmission();
    // Serial.println(value);

    return (1);
}


unsigned char Camera::readOv7670Reg(unsigned char regID) {
    unsigned char regData;
    Wire.beginTransmission(OV7670_ADDRESS); // 设备地址为0x21
    Wire.write(regID);

    // Serial.print("Read Register:wire return value:");

    int value = Wire.endTransmission(); //结束通信
    // Serial.println(value);

    Wire.requestFrom(OV7670_ADDRESS, 1);
    regData = Wire.read();

    return regData;
}

//OV7670寄存器配置，无逻辑意义，需根据厂家芯片手册了解具体含义
void Camera::setRegisters() {
    writeOv7670Reg(0x3a, 0x04);
    writeOv7670Reg(0x40, 0xd0);
    writeOv7670Reg(0x12, 0x14);
    writeOv7670Reg(0x32, 0x80);
    writeOv7670Reg(0x17, 0x16);
    writeOv7670Reg(0x18, 0x04);
    writeOv7670Reg(0x19, 0x02);
    writeOv7670Reg(0x1a, 0x7b);
    writeOv7670Reg(0x03, 0x06);
    writeOv7670Reg(0x0c, 0x00);
    writeOv7670Reg(0x3e, 0x00);
    writeOv7670Reg(0x70, 0x3a);

    // FIXME 此处有待调试，因为图片质量不好
    //如果调整为WriteOV7670Reg(0x71, 0x80);会显示8条彩色竖条，调试用。0x80和0x35只保留一条。
    // writeOv7670Reg(0x71, 0x80);
    writeOv7670Reg(0x71, 0x35);


    writeOv7670Reg(0x72, 0x11);
    writeOv7670Reg(0x73, 0x00);
    writeOv7670Reg(0xa2, 0x02);
    writeOv7670Reg(0x11, 0x81);

    writeOv7670Reg(0x7a, 0x20);
    writeOv7670Reg(0x7b, 0x1c);
    writeOv7670Reg(0x7c, 0x28);
    writeOv7670Reg(0x7d, 0x3c);
    writeOv7670Reg(0x7e, 0x55);
    writeOv7670Reg(0x7f, 0x68);
    writeOv7670Reg(0x80, 0x76);
    writeOv7670Reg(0x81, 0x80);
    writeOv7670Reg(0x82, 0x88);
    writeOv7670Reg(0x83, 0x8f);
    writeOv7670Reg(0x84, 0x96);
    writeOv7670Reg(0x85, 0xa3);
    writeOv7670Reg(0x86, 0xaf);
    writeOv7670Reg(0x87, 0xc4);
    writeOv7670Reg(0x88, 0xd7);
    writeOv7670Reg(0x89, 0xe8);

    writeOv7670Reg(0x13, 0xe0);
    writeOv7670Reg(0x00, 0x00);

    writeOv7670Reg(0x10, 0x00);
    writeOv7670Reg(0x0d, 0x00);
    writeOv7670Reg(0x14, 0x28);
    writeOv7670Reg(0xa5, 0x05);
    writeOv7670Reg(0xab, 0x07);
    writeOv7670Reg(0x24, 0x75);
    writeOv7670Reg(0x25, 0x63);
    writeOv7670Reg(0x26, 0xA5);
    writeOv7670Reg(0x9f, 0x78);
    writeOv7670Reg(0xa0, 0x68);
    writeOv7670Reg(0xa1, 0x03);
    writeOv7670Reg(0xa6, 0xdf);
    writeOv7670Reg(0xa7, 0xdf);
    writeOv7670Reg(0xa8, 0xf0);
    writeOv7670Reg(0xa9, 0x90);
    writeOv7670Reg(0xaa, 0x94);
    writeOv7670Reg(0x13, 0xe5);

    writeOv7670Reg(0x0e, 0x61);
    writeOv7670Reg(0x0f, 0x4b);
    writeOv7670Reg(0x16, 0x02);
    writeOv7670Reg(0x1e, 0x37);
    writeOv7670Reg(0x21, 0x02);
    writeOv7670Reg(0x22, 0x91);
    writeOv7670Reg(0x29, 0x07);
    writeOv7670Reg(0x33, 0x0b);
    writeOv7670Reg(0x35, 0x0b);
    writeOv7670Reg(0x37, 0x1d);
    writeOv7670Reg(0x38, 0x71);
    writeOv7670Reg(0x39, 0x2a);
    writeOv7670Reg(0x3c, 0x78);
    writeOv7670Reg(0x4d, 0x40);
    writeOv7670Reg(0x4e, 0x20);
    writeOv7670Reg(0x69, 0x00);
    writeOv7670Reg(0x6b, 0x60);
    writeOv7670Reg(0x74, 0x19);
    writeOv7670Reg(0x8d, 0x4f);
    writeOv7670Reg(0x8e, 0x00);
    writeOv7670Reg(0x8f, 0x00);
    writeOv7670Reg(0x90, 0x00);
    writeOv7670Reg(0x91, 0x00);
    writeOv7670Reg(0x92, 0x00);
    writeOv7670Reg(0x96, 0x00);
    writeOv7670Reg(0x9a, 0x80);
    writeOv7670Reg(0xb0, 0x84);
    writeOv7670Reg(0xb1, 0x0c);
    writeOv7670Reg(0xb2, 0x0e);
    writeOv7670Reg(0xb3, 0x82);
    writeOv7670Reg(0xb8, 0x0a);


    writeOv7670Reg(0x43, 0x14);
    writeOv7670Reg(0x44, 0xf0);
    writeOv7670Reg(0x45, 0x34);
    writeOv7670Reg(0x46, 0x58);
    writeOv7670Reg(0x47, 0x28);
    writeOv7670Reg(0x48, 0x3a);
    writeOv7670Reg(0x59, 0x88);
    writeOv7670Reg(0x5a, 0x88);
    writeOv7670Reg(0x5b, 0x44);
    writeOv7670Reg(0x5c, 0x67);
    writeOv7670Reg(0x5d, 0x49);
    writeOv7670Reg(0x5e, 0x0e);
    writeOv7670Reg(0x64, 0x04);
    writeOv7670Reg(0x65, 0x20);
    writeOv7670Reg(0x66, 0x05);
    writeOv7670Reg(0x94, 0x04);
    writeOv7670Reg(0x95, 0x08);
    writeOv7670Reg(0x6c, 0x0a);
    writeOv7670Reg(0x6d, 0x55);
    writeOv7670Reg(0x6e, 0x11);
    writeOv7670Reg(0x6f, 0x9f);
    writeOv7670Reg(0x6a, 0x40);
    writeOv7670Reg(0x01, 0x40);
    writeOv7670Reg(0x02, 0x40);
    writeOv7670Reg(0x13, 0xe7);
    writeOv7670Reg(0x15, 0x00);


    writeOv7670Reg(0x4f, 0x80);
    writeOv7670Reg(0x50, 0x80);
    writeOv7670Reg(0x51, 0x00);
    writeOv7670Reg(0x52, 0x22);
    writeOv7670Reg(0x53, 0x5e);
    writeOv7670Reg(0x54, 0x80);
    writeOv7670Reg(0x58, 0x9e);

    writeOv7670Reg(0x41, 0x08);
    writeOv7670Reg(0x3f, 0x00);
    writeOv7670Reg(0x75, 0x05);
    writeOv7670Reg(0x76, 0xe1);
    writeOv7670Reg(0x4c, 0x00);
    writeOv7670Reg(0x77, 0x01);
    writeOv7670Reg(0x3d, 0xc2);
    writeOv7670Reg(0x4b, 0x09);
    writeOv7670Reg(0xc9, 0x60);
    writeOv7670Reg(0x41, 0x38);
    writeOv7670Reg(0x56, 0x40);

    writeOv7670Reg(0x34, 0x11);
    writeOv7670Reg(0x3b, 0x02);

    writeOv7670Reg(0xa4, 0x89);
    writeOv7670Reg(0x96, 0x00);
    writeOv7670Reg(0x97, 0x30);
    writeOv7670Reg(0x98, 0x20);
    writeOv7670Reg(0x99, 0x30);
    writeOv7670Reg(0x9a, 0x84);
    writeOv7670Reg(0x9b, 0x29);
    writeOv7670Reg(0x9c, 0x03);
    writeOv7670Reg(0x9d, 0x4c);
    writeOv7670Reg(0x9e, 0x3f);
    writeOv7670Reg(0x78, 0x04);

    writeOv7670Reg(0x79, 0x01);
    writeOv7670Reg(0xc8, 0xf0);
    writeOv7670Reg(0x79, 0x0f);
    writeOv7670Reg(0xc8, 0x00);
    writeOv7670Reg(0x79, 0x10);
    writeOv7670Reg(0xc8, 0x7e);
    writeOv7670Reg(0x79, 0x0a);
    writeOv7670Reg(0xc8, 0x80);
    writeOv7670Reg(0x79, 0x0b);
    writeOv7670Reg(0xc8, 0x01);
    writeOv7670Reg(0x79, 0x0c);
    writeOv7670Reg(0xc8, 0x0f);
    writeOv7670Reg(0x79, 0x0d);
    writeOv7670Reg(0xc8, 0x20);
    writeOv7670Reg(0x79, 0x09);
    writeOv7670Reg(0xc8, 0x80);
    writeOv7670Reg(0x79, 0x02);
    writeOv7670Reg(0xc8, 0xc0);
    writeOv7670Reg(0x79, 0x03);
    writeOv7670Reg(0xc8, 0x40);
    writeOv7670Reg(0x79, 0x05);
    writeOv7670Reg(0xc8, 0x30);
    writeOv7670Reg(0x79, 0x26);
    writeOv7670Reg(0x09, 0x00);
}

void Camera::CameraISR() {
    uint16_t cnt;

    // 禁止中断（后面恢复）
    detachInterrupt(digitalPinToInterrupt(VSYNC));

    // 标记中断到来以便于轮询检查
    interruptComing = true;

    // 计数中断次数
    vsyncFlag++;
}


void Camera::checkVsyncFlag() {
    // Serial.println("check vsync flag is here........");
    // Serial.print("Interruption Called.vsync_flag:");
    // Serial.println(vsyncFlag);


    // 防止重复检验中断事件
    interruptComing = false;

    // 第一次触发中断，丢弃帧，用这一帧来配置
    if (vsyncFlag == 1) {
        // FIFO复位写指针
        digitalWrite(FIFO_WRST, HIGH);
        digitalWrite(FIFO_WRST, LOW);

        // 循环延时（delay函数不可用）
        for (i = 0; i < 100; i++);

        digitalWrite(FIFO_WRST, HIGH);
        digitalWrite(FIFO_WR, HIGH);
    }

    // 正式处理这一帧图像。
    if (vsyncFlag == 2) {
        digitalWrite(FIFO_WR, LOW);
        // FIXME 此处未验证，应该可以去掉
        detachInterrupt(digitalPinToInterrupt(VSYNC));

        //EXTI->EMR&=~(1<<4);

        //GPIOA->ODR ^= (1 << 3);

        digitalWrite(FIFO_RRST, LOW);

        digitalWrite(FIFO_RCLK, LOW);
        digitalWrite(FIFO_RCLK, HIGH);
        digitalWrite(FIFO_RCLK, LOW);
        digitalWrite(FIFO_RCLK, HIGH);

        digitalWrite(FIFO_RRST, HIGH);

        digitalWrite(FIFO_OE, LOW);

        // start transmitting
        Serial.write(0x01);
        Serial.write(0xFE);

        for (i = 0; i < 320; i++) {
            for (j = 0; j < 240; j++) {
                digitalWrite(FIFO_RCLK, LOW);
                digitalWrite(FIFO_RCLK, HIGH);
                t1 = readDataToByte();
                digitalWrite(FIFO_RCLK, LOW);
                digitalWrite(FIFO_RCLK, HIGH);
                t2 = readDataToByte();
                Serial.write(t2);
                Serial.write(t1);
            }
        }

        digitalWrite(FIFO_OE, HIGH);
    }

    pinMode(VSYNC, INPUT_PULLUP);

    if (vsyncFlag >= 2) {
        Serial.write(0xFE);
        Serial.write(0x01);

        // FIXME 此处是重复VSYNC，理论上删掉没问题，但未验证
        pinMode(VSYNC, INPUT_PULLUP);
        // 大于2次VSYNC后，后续中断就不理会了
        detachInterrupt(digitalPinToInterrupt(VSYNC));

        //也不需要轮询了。因为已经结束拍照片了。
        TaskScheduler &scheduler = TaskScheduler::getInstance();
        scheduler.deleteTask(taskID);

    } else {
        pinMode(VSYNC, INPUT_PULLUP);
        // 否则就恢复监听中断
        attachInterrupt(digitalPinToInterrupt(VSYNC), CameraISR, FALLING);//设置触发中断的端口，中断后运行的程序和触发模式
    }
}


unsigned char Camera::readDataToByte() {
    unsigned char tmp;
    unsigned char DATA7 = 0;
    unsigned char DATA6 = 0;
    unsigned char DATA5 = 0;
    unsigned char DATA4 = 0;
    unsigned char DATA3 = 0;
    unsigned char DATA2 = 0;
    unsigned char DATA1 = 0;
    unsigned char DATA0 = 0;
    tmp = 0;

    DATA7 = (unsigned char) digitalRead(D7);
    DATA6 = (unsigned char) digitalRead(D6);
    DATA5 = (unsigned char) digitalRead(D5);
    DATA4 = (unsigned char) digitalRead(D4);
    DATA3 = (unsigned char) digitalRead(D3);
    DATA2 = (unsigned char) digitalRead(D2);
    DATA1 = (unsigned char) digitalRead(D1);
    DATA0 = (unsigned char) digitalRead(D0);
    tmp = (DATA7 << 7) | (DATA6 << 6) | (DATA5 << 5) | (DATA4 << 4)\
 | (DATA3 << 3) | (DATA2 << 2) | (DATA1 << 1) | (DATA0 << 0);

    return tmp;
}

