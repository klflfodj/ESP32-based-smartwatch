#include <Arduino.h>
#include "LCD_GFX.h"
#include "ST7789.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "CST816S_Touch.h"

static lv_obj_t *page_main;
static lv_obj_t *page_settings;

// 点击按钮后进入设置页
void switch_to_settings(lv_event_t *e)
{
    lv_scr_load(page_settings);
}

// 点击按钮后回到主页面
void switch_to_main(lv_event_t *e)
{
    lv_scr_load(page_main);
}

void setup() 
{
  Serial.begin(115200);

  ST7789_Init();
  Touch_Init();
  lvgl_init();
  lv_touch_init();

  // 先创建两个空白页面，NULL 表示先不挂到屏幕上
  page_main = lv_obj_create(NULL);
  page_settings = lv_obj_create(NULL);

  // 主页面：放一个时间标签
  lv_obj_t *time_label = lv_label_create(page_main);
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

  // 主页面：放一个“设置”按钮
  lv_obj_t *btn_settings = lv_btn_create(page_main);
  lv_obj_align(btn_settings, LV_ALIGN_BOTTOM_MID, 0, -30);
  lv_obj_t *btn_settings_label = lv_label_create(btn_settings);
  lv_label_set_text(btn_settings_label, "Settings");
  lv_obj_add_event_cb(btn_settings, switch_to_settings, LV_EVENT_CLICKED, NULL);

  // 设置页：放一个“返回”按钮
  lv_obj_t *btn_back = lv_btn_create(page_settings);
  lv_obj_align(btn_back, LV_ALIGN_CENTER, 0, 0);
  lv_obj_t *btn_back_label = lv_label_create(btn_back);
  lv_label_set_text(btn_back_label, "Back");
  lv_obj_add_event_cb(btn_back, switch_to_main, LV_EVENT_CLICKED, NULL);

  // 最后把主页面加载到屏幕
  lv_scr_load(page_main);

}

void loop() 
{
  lv_tick_inc(5);     // 每 5ms 告诉 LVGL 时间过了多久
  lv_timer_handler(); // LVGL 处理刷新和动画
  delay(5);
}
