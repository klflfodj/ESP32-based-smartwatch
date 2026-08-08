#include <Arduino.h>
#include "ST7789.h"

void setup() 
{
  Serial.begin(115200);

  ST7789_Init();

  Serial.println("ST7789 Init OK");

  ST7789_FillScreen(LCD_RED);

  Serial.println("RED TEST OK");
}

void loop() 
{

}
