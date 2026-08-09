#include "ST7789.h"

extern SPIClass LCD_SPI;

SPI_Config_typedef LCD_SPI_Config = {LCD_CS, LCD_DC, LCD_RST, LCD_SCK, LCD_MOSI, LCD_SPI_SPEED};

// -----------------------
// ST7789 硬件复位
// -----------------------
// 功能介绍：
// 对 ST7789 进行硬件复位
// -----------------------
static void ST7789_Reset(void)
{
    digitalWrite(LCD_RST, HIGH);
    delay(10);

    digitalWrite(LCD_RST, LOW);
    delay(20);

    digitalWrite(LCD_RST, HIGH);
    delay(120);
}

// -----------------------
// 初始化 ST7789
// -----------------------
// 功能介绍：
// 初始化 ST7789 显示控制器
// -----------------------
void ST7789_Init(void)
{
    SPI_Init(&LCD_SPI_Config);
    ST7789_Reset();

    delay(150);

    // 退出深度睡眠模式
    SPI_WriteCommand(&LCD_SPI_Config, 0x11);// 0x11 → 退出深度睡眠模式
    delay(120);

    // 设置颜色模式为 RGB565
    SPI_WriteCommand(&LCD_SPI_Config, 0x3A);// 0x3A → 颜色模式设置
    // 0x55 → RGB565
    SPI_WriteData(&LCD_SPI_Config, 0x55);
    delay(10);

    // 设置显示模式为正常显示
    SPI_WriteCommand(&LCD_SPI_Config, 0x36);// 0x36 → 显示模式设置
    // 0xC0 → 正常显示
    SPI_WriteData(&LCD_SPI_Config, 0xC0);
    delay(10);

    // 显示反转
    SPI_WriteCommand(&LCD_SPI_Config, 0x21);
    delay(10);

    // 使能显示模式
    SPI_WriteCommand(&LCD_SPI_Config, 0x29);

    delay(100);
}

// -----------------------
// 设置显示区域
// -----------------------
// 功能介绍：
// 设置 ST7789 接下来要写入的显示区域
//
// x0,y0：起始坐标
// x1,y1：结束坐标
// -----------------------
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    y0 += LCD_Y_OFFSET;
    y1 += LCD_Y_OFFSET;
    x0 += LCD_X_OFFSET;
    x1 += LCD_X_OFFSET;

    // -----------------------
    // 设置 x 坐标范围
    // -----------------------
    SPI_WriteCommand(&LCD_SPI_Config, 0x2A);// 0x2A → 设置地址范围

    SPI_WriteData(&LCD_SPI_Config, x0 >> 8);
    SPI_WriteData(&LCD_SPI_Config, x0 & 0xFF);

    SPI_WriteData(&LCD_SPI_Config, x1 >> 8);
    SPI_WriteData(&LCD_SPI_Config, x1 & 0xFF);

    // -----------------------
    // 设置 y 坐标范围
    // -----------------------
    SPI_WriteCommand(&LCD_SPI_Config, 0x2B);// 0x2B → 设置地址范围

    SPI_WriteData(&LCD_SPI_Config, y0 >> 8);
    SPI_WriteData(&LCD_SPI_Config, y0 & 0xFF);

    SPI_WriteData(&LCD_SPI_Config, y1 >> 8);
    SPI_WriteData(&LCD_SPI_Config, y1 & 0xFF);

    // -----------------------
    // 开始写入显存
    // -----------------------
    SPI_WriteCommand(&LCD_SPI_Config, 0x2C);// 0x2C → 开始写入显存

}

void ST7789_BeginWrite(void)
{
    digitalWrite(LCD_DC, HIGH);

    digitalWrite(LCD_CS, LOW);

    LCD_SPI.beginTransaction(SPISettings(LCD_SPI_Config.SPI_Speed, MSBFIRST, SPI_MODE0));

}

void ST7789_WriteColor(uint16_t color)
{
    LCD_SPI.transfer(color >> 8);
    LCD_SPI.transfer(color & 0xFF);
}

void ST7789_EndWrite(void)
{
    LCD_SPI.endTransaction();

    digitalWrite(LCD_CS, HIGH);
}

void ST7789_DrawPixelFast(uint16_t x, uint16_t y, uint16_t color)
{
    ST7789_SetAddressWindow(x,y,x,y);
    ST7789_WriteColor(color);
}

