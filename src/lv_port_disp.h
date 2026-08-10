#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#include "lvgl.h"

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void lvgl_init();
void lv_indev_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
void lv_touch_init(void);

#endif
