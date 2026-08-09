#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <Arduino.h>
#include <SPI.h>

extern SPIClass LCD_SPI;

void SPI_Init(uint8_t CS_Pin, uint8_t DC_Pin, uint8_t RST_Pin, uint8_t SCK_Pin, uint8_t MOSI_Pin);

void SPI_WriteCommand(uint8_t DC_Pin, uint8_t CS_Pin, uint32_t SPI_Speed
                        , uint8_t cmd);
void SPI_WriteData(uint8_t DC_Pin, uint8_t CS_Pin, uint32_t SPI_Speed
                    , uint8_t data);
void SPI_WriteDataBuffer(uint8_t DC_Pin, uint8_t CS_Pin, uint32_t SPI_Speed
                            , const uint8_t *data, uint32_t len);


#endif