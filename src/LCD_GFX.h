#ifndef LCD_GFX_H
#define LCD_GFX_H

#include <Arduino.h>
#include "ST7789.h"


// 填充屏幕
void LCD_FillScreen(uint16_t color);

/************** 绘制函数 **************/

// 绘制实心矩形
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
// 绘制像素点
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
// 绘制线
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

#endif
