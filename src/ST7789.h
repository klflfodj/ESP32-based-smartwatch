#ifndef ST7789_H
#define ST7789_H

#include <Arduino.h>
#include <SPI.h>

/*SPI 时钟频率 */
#define LCD_SPI_SPEED     24000000 // 26MHz极限频率

/* 数据缓冲区像素上限设置 */
#define LCD_BUFFER_PIXELS 512

/* 屏幕分辨率 */
#define LCD_WIDTH         240
#define LCD_HEIGHT        280

/* 屏幕 Y 偏移 */
#define LCD_Y_OFFSET      20

/* 屏幕 X 偏移 */
#define LCD_X_OFFSET      0

/* LCD 引脚 */
#define LCD_SCK           18 // 时钟引脚
#define LCD_MOSI          23 // 主输出从输入引脚
#define LCD_CS            5  // 选中引脚
#define LCD_DC            15 // 数据/命令引脚
#define LCD_RST           21 // 复位引脚

/* RGB565 颜色 */
#define LCD_BLACK         0x0000
#define LCD_WHITE         0xFFFF
#define LCD_RED           0xF800
#define LCD_GREEN         0x07E0
#define LCD_BLUE          0x001F

/* 主要函数 */

// 初始化 ST7789
void ST7789_Init(void);

// 发送命令
void ST7789_WriteCommand(uint8_t cmd);
// 发送数据
void ST7789_WriteData(uint8_t data);
void ST7789_WriteDataBuffer(const uint8_t *data, uint32_t len);

// 设置地址范围
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

// 填充屏幕
void ST7789_FillScreen(uint16_t color);

/************** 绘制函数 **************/

// 绘制实心矩形
void ST7789_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
// 绘制像素点
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
// 绘制线
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);


#endif
