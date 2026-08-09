#ifndef CST816S_H
#define CST816S_H

#include <Arduino.h>
#include <Wire.h>

#define TOUCH_SDA 32
#define TOUCH_SCL 33
#define TOUCH_RST 25// 重置引脚
#define TOUCH_INT 26// 中断引脚

#define CST816S_ADDR 0x15// I2C 地址

// 触摸初始化
void Touch_Init(void);

// 读取触摸坐标，返回 true 表示有手指按下
// x, y 是输出参数，读取结果会填进去
bool Touch_GetXY(uint16_t *x, uint16_t *y);

#endif
