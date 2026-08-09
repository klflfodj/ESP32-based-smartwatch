#include "SPI_Driver.h"

SPIClass LCD_SPI(VSPI);// 初始化 SPI 通信

// -----------------------
// 初始化 SPI
// -----------------------
// 功能介绍：
// 初始化 ST7789 SPI 通信
// -----------------------
void SPI_Init(SPI_Config_typedef *config)
{
    pinMode(config->CS_Pin, OUTPUT);
    pinMode(config->DC_Pin, OUTPUT);
    pinMode(config->RST_Pin, OUTPUT);

    digitalWrite(config->CS_Pin, HIGH);
    digitalWrite(config->DC_Pin, HIGH);
    digitalWrite(config->RST_Pin, HIGH);

    LCD_SPI.begin(config->SCK_Pin, -1, config->MOSI_Pin, config->CS_Pin);

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
void SPI_WriteCommand(SPI_Config_typedef *config, uint8_t cmd)
{
    //DC = LOW  → 命令
    //DC = HIGH → 数据
    digitalWrite(config->DC_Pin, LOW);

    LCD_SPI.beginTransaction(SPISettings(config->SPI_Speed, MSBFIRST, SPI_MODE0));

    digitalWrite(config->CS_Pin, LOW);

    LCD_SPI.transfer(cmd);

    digitalWrite(config->CS_Pin, HIGH);

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
void SPI_WriteData(SPI_Config_typedef *config, uint8_t data)
{
    //DC = LOW  → 命令
    //DC = HIGH → 数据
    digitalWrite(config->DC_Pin, HIGH);

    LCD_SPI.beginTransaction(SPISettings(config->SPI_Speed, MSBFIRST, SPI_MODE0));

    digitalWrite(config->CS_Pin, LOW);

    LCD_SPI.transfer(data);

    digitalWrite(config->CS_Pin, HIGH);

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
void SPI_WriteDataBuffer(SPI_Config_typedef *config, const uint8_t *data, uint32_t len)
{
    digitalWrite(config->DC_Pin, HIGH);
    digitalWrite(config->CS_Pin, LOW);

    LCD_SPI.beginTransaction(SPISettings(config->SPI_Speed, MSBFIRST, SPI_MODE0));

    LCD_SPI.writeBytes(data, len);

    LCD_SPI.endTransaction();

    digitalWrite(config->CS_Pin, HIGH);

}
