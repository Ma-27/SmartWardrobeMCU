/**
 * @description: Camera class for controlling camera module operations
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/3/8 下午 04:43

  效果：驱动摄像头OV7670带FIFO模块，拍摄1帧图片，并将图片数据通过串口上传至电脑(暂时的，图片目前保存在FIFO寄存器中）。
  分辨率320x240.
  使用Arduino Due 和OV7670 FIFO版（AL422B)。图片采用RGB小端存储

  CONNECTIONS:
  OV7670 D0 -> Arduino Due 44
  OV7670 D1 -> Arduino Due 45
  OV7670 D2 -> Arduino Due 42
  OV7670 D3 -> Arduino Due 43
  OV7670 D4 -> Arduino Due 40
  OV7670 D5 -> Arduino Due 41
  OV7670 D6 -> Arduino Due 38
  OV7670 D7 -> Arduino Due 39

  OV7670  FIFIO_RCK     ->  Arduino  46
  OV7670  FIFIO_OE      ->  Arduino  50
  OV7670  FIFIO_WR      ->  Arduino  47
  OV7670  FIFIO_RRST    ->  Arduino  48
  OV7670  SIO_C         ->  Arduino  SCL （管脚不可调整为其他管脚，尤其是不可以调整为SCL1,Arduino Due会有Bug）
  OV7670  SIO_D         ->  Arduino  SDA （管脚不可调整为其他管脚，尤其是不可以调整为SDA1,Arduino Due会有Bug）
  CameraISR  FIFIO_WRST    ->  Arduino  49
  OV7670  VSYNC         ->  Arduino  3   （可调为其他允许外部中断的管脚）
  OV7670  RESET         ->  Arduino  51
  OV7670  3.3V          ->  Arduino  3.3V
  OV7670  GND           ->  Arduino  GND
  其余摄像头针脚可以不接(悬空)。
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include "utility/ProjectConfig.h"
#include <Wire.h> // I2C 协议必不可少的库
#include "core/TaskScheduler.h"
#include "core/CommandListener.h"

#define OV7670_ADDRESS    0x21  //摄像头设备地址
#define VSYNC      3

#define D0         44
#define D1         45
#define D2         42
#define D3         43
#define D4         40
#define D5         41
#define D6         38
#define D7         39

#define RESET      51

#define FIFO_WRST  49
#define FIFO_WR    47
#define FIFO_RRST  48
#define FIFO_RCLK   46
#define FIFO_OE    50

class Camera : public CommandListener {
public:
    // 删除拷贝构造函数和赋值操作符，确保单例的唯一性
    Camera(const Camera &) = delete;

    void operator=(const Camera &) = delete;

    // 提供一个公共的访问方法
    static Camera *getInstance();

    /**
    * 捕获图片的代码
    准确的说是使用寄存器配置照相参数，然后使用中断+轮询来读摄像头寄存器中的图片数据。
    例如：触发相机模块的拍照功能，然后将图片数据存储到SD卡或其他存储介质
    初始化摄像头
    */
    void captureImage();

    // 解析命令
    bool parseCommand(const String &command) override;

    // 具体解析是哪个负责执行命令，派发给相应的监听器
    bool dispatchCommand(String &command, const String &tag, CommandListener *listener) override;



private:
    static Camera *instance; // 静态私有实例指针

    unsigned char tmp;
    unsigned char t1, t2;
    static volatile int vsyncFlag;
    volatile int i = 0, j = 0;
    static volatile bool interruptComing; // 新增一个全局标志,用来标记是否进入中断

    // 调度进程轮询的ID号码。
    int taskID;

    // 私有化构造函数
    Camera();

    // 初始化相机的真正函数
    void initCamera();


    /**
    *
    * 向OV7670寄存器写数据
    * @param regID 寄存器地址编号
    * @param regDat 寄存器值
    * @return
    */
    unsigned char writeOv7670Reg(unsigned char regID, unsigned char regDat);

    /**
    * 从OV7670寄存器读数据
    * @param regID 寄存器地址编号
    * @return 读到的数据
    */
    unsigned char readOv7670Reg(unsigned char regID);

    /**
     * 初始化CMOS。
     * 主要包含两个步骤：
     * 1.给寄存器0x12设置值0x80，来复位所有寄存器。
     * 2.根据手册设置所有的寄存器
     * @return
     */
    unsigned char initCMOS();

    void setRegisters();

    unsigned char readDataToByte();

    void ioInit();

    void checkVsyncFlag();

    static void CameraISR();

    void checkInterruption(bool enabled);
};

#endif // CAMERA_H
