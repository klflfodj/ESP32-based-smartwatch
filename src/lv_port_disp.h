#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#include "lvgl.h"
#include "ST7789.h"
#include "CST816S_Touch.h"

#define LCD_HEIGHT_T  LCD_HEIGHT/10

static lv_color_t draw_buf_1[LCD_WIDTH * LCD_HEIGHT_T];
static lv_color_t draw_buf_2[LCD_WIDTH * LCD_HEIGHT_T];


static lv_disp_draw_buf_t draw_buf;// 绘制缓冲区，用于存储需要刷新的矩形区域
static lv_disp_drv_t disp_drv;// 显示驱动对象，用于刷新屏幕
static lv_indev_drv_t indev_drv;// 输入设备对象，用于读触摸屏数据

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void lvgl_init();
void lv_indev_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
void lv_touch_init(void);

#endif
