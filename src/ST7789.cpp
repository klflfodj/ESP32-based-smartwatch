#include "ST7789.h"

SPIClass LCD_SPI(VSPI);

// -----------------------
// 初始化 SPI
// -----------------------
// 功能介绍：
// 初始化 ST7789 SPI 通信
// -----------------------
static void LCD_SPI_Init(void)
{
    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_DC, OUTPUT);
    pinMode(LCD_RST, OUTPUT);

    digitalWrite(LCD_CS, HIGH);
    digitalWrite(LCD_DC, HIGH);
    digitalWrite(LCD_RST, HIGH);

    LCD_SPI.begin(LCD_SCK, -1, LCD_MOSI, LCD_CS);

}

// -----------------------
// 发送命令
// -----------------------
// 功能介绍：
// 发送 ST7789 命令
// -----------------------
// 参数：
// cmd - 命令字节
// -----------------------
void ST7789_WriteCommand(uint8_t cmd)
{
    //DC = LOW  → 命令
    //DC = HIGH → 数据
    digitalWrite(LCD_DC, LOW);

    LCD_SPI.beginTransaction(SPISettings(LCD_SPI_SPEED, MSBFIRST, SPI_MODE0));

    digitalWrite(LCD_CS, LOW);

    LCD_SPI.transfer(cmd);

    digitalWrite(LCD_CS, HIGH);

    LCD_SPI.endTransaction();
}

// -----------------------
// 发送数据
// -----------------------
// 功能介绍：
// 发送 ST7789 数据
// -----------------------
// 参数：
// data - 数据字节
// -----------------------
void ST7789_WriteData(uint8_t data)
{
    //DC = LOW  → 命令
    //DC = HIGH → 数据
    digitalWrite(LCD_DC, HIGH);

    LCD_SPI.beginTransaction(SPISettings(LCD_SPI_SPEED, MSBFIRST, SPI_MODE0));

    digitalWrite(LCD_CS, LOW);

    LCD_SPI.transfer(data);

    digitalWrite(LCD_CS, HIGH);

    LCD_SPI.endTransaction();
}

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
    LCD_SPI_Init();
    ST7789_Reset();

    delay(150);

    // 退出深度睡眠模式
    ST7789_WriteCommand(0x11);// 0x11 → 退出深度睡眠模式
    delay(120);

    // 设置颜色模式为 RGB565
    ST7789_WriteCommand(0x3A);// 0x3A → 颜色模式设置
    // 0x55 → RGB565
    ST7789_WriteData(0x55);
    delay(10);

    // 设置显示模式为正常显示
    ST7789_WriteCommand(0x36);// 0x36 → 显示模式设置
    // 0xC0 → 正常显示
    ST7789_WriteData(0xC0);
    delay(10);

    // 显示反转
    ST7789_WriteCommand(0x21);
    delay(10);

    // 使能显示模式
    ST7789_WriteCommand(0x29);

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
    ST7789_WriteCommand(0x2A);// 0x2A → 设置地址范围

    ST7789_WriteData(x0 >> 8);
    ST7789_WriteData(x0 & 0xFF);

    ST7789_WriteData(x1 >> 8);
    ST7789_WriteData(x1 & 0xFF);

    // -----------------------
    // 设置 y 坐标范围
    // -----------------------
    ST7789_WriteCommand(0x2B);// 0x2B → 设置地址范围

    ST7789_WriteData(y0 >> 8);
    ST7789_WriteData(y0 & 0xFF);

    ST7789_WriteData(y1 >> 8);
    ST7789_WriteData(y1 & 0xFF);

    // -----------------------
    // 开始写入显存
    // -----------------------
    ST7789_WriteCommand(0x2C);// 0x2C → 开始写入显存

}

// -----------------------
// 发送数据缓冲区
// -----------------------
// 功能介绍：
// 发送 ST7789 数据缓冲区
// -----------------------
// 参数：
// data - 数据指针
// len  - 数据长度
// -----------------------
void ST7789_WriteDataBuffer(const uint8_t *data, uint32_t len)
{
    digitalWrite(LCD_DC, HIGH);
    digitalWrite(LCD_CS, LOW);

    LCD_SPI.beginTransaction(SPISettings(LCD_SPI_SPEED, MSBFIRST, SPI_MODE0));

    LCD_SPI.writeBytes(data, len);

    LCD_SPI.endTransaction();

    digitalWrite(LCD_CS, HIGH);

}

/************************ 主要绘图函数 *************************/

// -----------------------
// 绘制一个像素
// -----------------------
// 功能介绍：
// 在指定坐标绘制一个 RGB565 像素
//
// x     → X 坐标
// y     → Y 坐标
// color → RGB565颜色
// -----------------------
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    ST7789_SetAddressWindow(x, y, x, y);

    ST7789_WriteData(color >> 8);

    ST7789_WriteData(color & 0xFF);
}

// -----------------------
// 填充屏幕
// -----------------------
// 功能介绍：
// 填充屏幕为指定颜色
//
// color → RGB565颜色
// -----------------------
void ST7789_FillScreen(uint16_t color)
{
    ST7789_SetAddressWindow(0,0,LCD_WIDTH - 1,LCD_HEIGHT - 1);

    digitalWrite(LCD_DC, HIGH);
    digitalWrite(LCD_CS, LOW);

    LCD_SPI.beginTransaction(SPISettings(LCD_SPI_SPEED, MSBFIRST, SPI_MODE0));

    for(uint32_t i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++)
    {
        LCD_SPI.transfer(color >> 8);
        LCD_SPI.transfer(color & 0xFF);
    }

    LCD_SPI.endTransaction();

    digitalWrite(LCD_CS, HIGH);
}

void ST7789_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
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
        ST7789_WriteDataBuffer(buffer, pixelsTosend * 2);

        // 更新剩余像素数量
        totalPixels -= pixelsTosend;// 减去已发送像素数量

    }


}

