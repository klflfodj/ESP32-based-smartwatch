#include "SPI_Driver.h"

SPIClass LCD_SPI(VSPI);// 初始化 SPI 通信

// -----------------------
// 初始化 SPI
// -----------------------
// 功能介绍：
// 初始化 ST7789 SPI 通信
// -----------------------
void SPI_Init(uint8_t CS_Pin, uint8_t DC_Pin, uint8_t RST_Pin, uint8_t SCK_Pin, uint8_t MOSI_Pin)
{
    pinMode(CS_Pin, OUTPUT);
    pinMode(DC_Pin, OUTPUT);
    pinMode(RST_Pin, OUTPUT);

    digitalWrite(CS_Pin, HIGH);
    digitalWrite(DC_Pin, HIGH);
    digitalWrite(RST_Pin, HIGH);

    LCD_SPI.begin(SCK_Pin, -1, MOSI_Pin, CS_Pin);

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
void SPI_WriteCommand(uint8_t DC_Pin, uint8_t CS_Pin, uint32_t SPI_Speed
                        , uint8_t cmd)
{
    //DC = LOW  → 命令
    //DC = HIGH → 数据
    digitalWrite(DC_Pin, LOW);

    LCD_SPI.beginTransaction(SPISettings(SPI_Speed, MSBFIRST, SPI_MODE0));

    digitalWrite(CS_Pin, LOW);

    LCD_SPI.transfer(cmd);

    digitalWrite(CS_Pin, HIGH);

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
void SPI_WriteData(uint8_t DC_Pin, uint8_t CS_Pin, uint32_t SPI_Speed
                    , uint8_t data)
{
    //DC = LOW  → 命令
    //DC = HIGH → 数据
    digitalWrite(DC_Pin, HIGH);

    LCD_SPI.beginTransaction(SPISettings(SPI_Speed, MSBFIRST, SPI_MODE0));

    digitalWrite(CS_Pin, LOW);

    LCD_SPI.transfer(data);

    digitalWrite(CS_Pin, HIGH);

    LCD_SPI.endTransaction();
}

// -----------------------
// 连续发送数据
// -----------------------
// 功能介绍：
// 连续发送数据
// -----------------------
// 参数：
// data - 数据指针
// len  - 数据长度
// -----------------------
void SPI_WriteDataBuffer(uint8_t DC_Pin, uint8_t CS_Pin, uint32_t SPI_Speed
                            , const uint8_t *data, uint32_t len)
{
    digitalWrite(DC_Pin, HIGH);
    digitalWrite(CS_Pin, LOW);

    LCD_SPI.beginTransaction(SPISettings(SPI_Speed, MSBFIRST, SPI_MODE0));

    LCD_SPI.writeBytes(data, len);

    LCD_SPI.endTransaction();

    digitalWrite(CS_Pin, HIGH);

}
