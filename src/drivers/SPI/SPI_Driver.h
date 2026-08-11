#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <Arduino.h>
#include <SPI.h>

typedef struct
{
    uint8_t CS_Pin;
    uint8_t DC_Pin;
    uint8_t RST_Pin;
    uint8_t SCK_Pin;
    uint8_t MOSI_Pin;
    uint32_t SPI_Speed;
} SPI_Config_typedef;

extern SPIClass LCD_SPI;

void SPI_Init(SPI_Config_typedef *config);

void SPI_WriteCommand(SPI_Config_typedef *config, uint8_t cmd);
void SPI_WriteData(SPI_Config_typedef *config, uint8_t data);
void SPI_WriteDataBuffer(SPI_Config_typedef *config, const uint8_t *data, uint32_t len);


#endif