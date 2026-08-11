#include <Arduino.h>
#include "draw/LCD_GFX.h"
#include "drivers/SPI/ST7789.h"
#include "lvgl.h"
#include "draw/lv_port_disp.h"
#include "drivers/IIC/CST816s_Touch.h"
#include "UI.h"

void setup() 
{
  Serial.begin(115200);

  ST7789_Init();
  Touch_Init();
  lvgl_init();
  lv_touch_init();

  UI_init();
}

void loop() 
{
  lv_tick_inc(5);     // 每 5ms 告诉 LVGL 时间过了多久
  lv_timer_handler(); // LVGL 处理刷新和动画
  delay(5);
}
