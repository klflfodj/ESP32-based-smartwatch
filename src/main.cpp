#include <Arduino.h>
#include "LCD_GFX.h"
#include "ST7789.h"
#include "lvgl.h"
#include "lv_port_disp.h"

void setup() 
{
  Serial.begin(115200);

  ST7789_Init();
  lvgl_init();

  // 测试页面：居中显示一行文字
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello LVGL");
  lv_obj_center(label);

}

void loop() 
{
  lv_tick_inc(5);     // 每 5ms 告诉 LVGL 时间过了多久
  lv_timer_handler(); // LVGL 处理刷新和动画
  delay(5);
}
