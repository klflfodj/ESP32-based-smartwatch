#include <Arduino.h>
#include "ST7789.h"
#include "LCD_GFX.h"


void setup() 
{
  Serial.begin(115200);

  ST7789_Init();

  Serial.println("ST7789 Init OK");

  LCD_FillScreen(LCD_WHITE);

  LCD_FillRect(50, 50, 100, 100, LCD_RED);

  Serial.println("RED TEST OK");
}

void loop() 
{

}
