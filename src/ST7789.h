#ifndef ST7789_H
#define ST7789_H

#include <Arduino.h>
#include "drivers/SPI/SPI_Driver.h"

// SPI 配置
extern SPI_Config_typedef LCD_SPI_Config;

/*SPI 时钟频率 */
/* 40MHz -> 80MHz：整屏刷新的 SPI 传输时间减半，翻页和滑动明显更跟手
   如果换线后出现花屏/闪烁，把它改回 40000000 */
#define LCD_SPI_SPEED     80000000

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

// 初始化 ST7789
void ST7789_Init(void);
// 设置地址范围
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
// 开始连续写入像素
void ST7789_BeginWrite(void);
// 连续写一个颜色
 void ST7789_WriteColor(uint16_t color);
// 结束写入
void ST7789_EndWrite(void);

#endif
