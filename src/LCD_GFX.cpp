#include "LCD_GFX.h"

// -----------------------
// 绘制一个像素点
// -----------------------
// 功能介绍：
// 在指定坐标绘制一个 RGB565 像素
//
// x     → X 坐标
// y     → Y 坐标
// color → RGB565颜色
// -----------------------
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    ST7789_SetAddressWindow(x, y, x, y);

    ST7789_BeginWrite();
    ST7789_WriteColor(color);
    ST7789_EndWrite();
}

// -----------------------
// 填充屏幕
// -----------------------
// 功能介绍：
// 填充屏幕为指定颜色
//
// color → RGB565颜色
// -----------------------
void LCD_FillScreen(uint16_t color)
{
    LCD_FillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

// -----------------------
// 绘制实心矩形
// -----------------------
// 功能介绍：
// 绘制一个实心矩形
//
// x → 矩形左上角 X 坐标
// y     → 矩形左上角 Y 坐标
// w     → 矩形宽度
// h     → 矩形高度
// color → RGB565颜色
// -----------------------
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{

    // 完全在屏幕外就直接返回
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
    {
        return;
    }

    // 超出右边就把宽度截短
    if (x + w > LCD_WIDTH)
    {
        w = LCD_WIDTH - x;
    }

    // 超出下边就把高度截短
    if (y + h > LCD_HEIGHT)
    {
        h = LCD_HEIGHT - y;
    }


    // 计算需要发送的像素数量
    uint32_t totalPixels = (uint32_t)w * h;

    // 设置显示区域
    ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);

    // 创建 512 像素的缓冲区
    uint8_t buffer[LCD_BUFFER_PIXELS * 2];

    // ------------------------------------------------
    // 先把 512 个像素全部填成指定颜色
    // ------------------------------------------------
    for(uint32_t i = 0; i < LCD_BUFFER_PIXELS; i++)
    {
        buffer[i * 2] = color >> 8;
        buffer[i * 2 + 1] = color & 0xFF;
    }

    // ------------------------------------------------
    // 分批发送
    // ------------------------------------------------
    while(totalPixels > 0)
    {
        uint32_t pixelsTosend;

        if(totalPixels >= LCD_BUFFER_PIXELS)// 如果剩余像素数量大于缓冲区像素上限
        {
            pixelsTosend = LCD_BUFFER_PIXELS;// 每次发送 512 个像素
        }
        else
        {
            pixelsTosend = totalPixels;// 最后一次发送剩余像素
        }

        // 发送这一批像素
        SPI_WriteDataBuffer(&LCD_SPI_Config, buffer, pixelsTosend * 2);

        // 更新剩余像素数量
        totalPixels -= pixelsTosend;// 减去已发送像素数量

    }

}

// -----------------------
// 绘制线
// -----------------------
// 功能介绍：
// 绘制一条线
//
// x0 → 起始点 X 坐标
// y0 → 起始点 Y 坐标
// x1 → 终点 X 坐标
// y1 → 终点 Y 坐标
// color → RGB565颜色
// -----------------------
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    // 完全在屏幕外就直接返回
    if (x0 >= LCD_WIDTH || y0 >= LCD_HEIGHT || x1 >= LCD_WIDTH || y1 >= LCD_HEIGHT)
    {
        return;
    }

    // 超出右边就把宽度截短
    if (x0 > LCD_WIDTH)
    {
        x0 = LCD_WIDTH;
    }

    if (x1 > LCD_WIDTH)
    {
        x1 = LCD_WIDTH;
    }


    // 超出下边就把高度截短
    if (y0 > LCD_HEIGHT)
    {
        y0 = LCD_HEIGHT;
    }

    if (y1 > LCD_HEIGHT)
    {
        y1 = LCD_HEIGHT;
    }


    // 计算线的长度
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    if (y0 == y1)
    {
        // 终点在左边时交换，保证从左往右画
        if (x0 > x1)
        {
            uint16_t t = x0;
            x0 = x1;
            x1 = t;
        }
    
        LCD_DrawFastHLine(x0, y0, x1 - x0 + 1, color);
        return;
    }

    if (x0 == x1)
    {
        // 终点在上方时交换，保证从上往下画
        if (y0 > y1)
        {
            uint16_t t = y0;
            y0 = y1;
            y1 = t;
        }
        LCD_DrawFastVLine(x0, y0, y1 - y0 + 1, color);
        return;
    }

    // 计算线的增量
    // 判断线的方向
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    // 计算错误项
    int err = dx - dy;

    while(1)
    {
        LCD_DrawPixel(x0, y0, color);

        // 判断是否到达终点
        if(x0 == x1 && y0 == y1)
        {
            break;
        }

        // 扩大当前误差
        int e2 = 2 * err;

        // x方向是否前进
        if(e2 >= -dy)
        {
            err -= dy;
            x0 += sx;
        }

        // y方向是否前进
        if(e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }

    }

}

// -----------------------
// 绘制水平线
// -----------------------
// 功能介绍：
// 绘制一条水平线
//
// x → 起始点 X 坐标
// y → 起始点 Y 坐标
// w → 线的宽度
// color → RGB565颜色
// -----------------------
void LCD_DrawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    // 完全在屏幕外就直接返回
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
    {
        return;
    }

    // 超出右边就把宽度截短
    if (x + w > LCD_WIDTH)
    {
        w = LCD_WIDTH - x;
    }

    // 超出下边就把高度截短
    if (y > LCD_HEIGHT)
    {
        y = LCD_HEIGHT;
    }

    ST7789_SetAddressWindow(x, y, x + w - 1, y);

    ST7789_BeginWrite();

    while(w--)
    {
        ST7789_WriteColor(color);
    }
    ST7789_EndWrite();
    
}

// -----------------------
// 绘制垂直线
// -----------------------
// 功能介绍：
// 绘制一条垂直线
//
// x → 起始点 X 坐标
// y → 起始点 Y 坐标
// h → 线的高度
// color → RGB565颜色
// -----------------------
void LCD_DrawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{

    // 完全在屏幕外就直接返回
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
    {
        return;
    }

    // 超出右边就把宽度截短
    if (x > LCD_WIDTH)
    {
        x = LCD_WIDTH;
    }

    // 超出下边就把高度截短
    if (y + h > LCD_HEIGHT)
    {
        h = LCD_HEIGHT - y;
    }

    ST7789_SetAddressWindow(x, y, x, y + h - 1);

    ST7789_BeginWrite();

    while(h--)
    {
        ST7789_WriteColor(color);
    }
    ST7789_EndWrite();
}



