#include "lvgl.h"
#include "ST7789.h"

// -----------------------
// 功能介绍：
// 两个绘制缓冲区，每个约 1/10 屏幕，ESP32 内存够用
// -----------------------
// draw_buf_1 → 第一个绘制缓冲区
// draw_buf_2 → 第二个绘制缓冲区
// -----------------------
// 注意：
// 两个缓冲区的大小必须是 LCD_WIDTH * 28，否则会导致内存溢出
// -----------------------
static lv_color_t draw_buf_1[LCD_WIDTH * 28];
static lv_color_t draw_buf_2[LCD_WIDTH * 28];

// -----------------------
// 功能介绍：
// 刷新 LVGL 绘制的矩形区域到屏幕
// -----------------------
// disp_drv → 显示驱动对象
// area → 需要刷新的矩形区域
// color_p → 这一块区域每个像素的颜色数据
// -----------------------
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    // 取出要刷新的矩形范围
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;
    uint32_t pixel_count = (uint32_t)(x2 - x1 + 1) * (y2 - y1 + 1);

    // 设置屏幕写入区域并开始连续写入
    ST7789_SetAddressWindow(x1, y1, x2, y2);
    ST7789_BeginWrite();

    // 注意：lv_conf.h 里 LV_COLOR_16_SWAP 要保持 0
    // 因为 ST7789_WriteColor 会先发颜色高字节，再发低字节
    for (uint32_t i = 0; i < pixel_count; i++)
    {
        ST7789_WriteColor(color_p[i].full);
    }

    ST7789_EndWrite();

    // 通知 LVGL 这一块已经刷新完成
    lv_disp_flush_ready(disp_drv);
}


static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;


// -----------------------
// 功能介绍：
// 初始化 LVGL 显示驱动
// -----------------------
// draw_buf → LVGL 绘制缓冲区
// disp_drv → LVGL 显示驱动对象
// -----------------------
void lvgl_init()
{
    lv_init(); // 初始化 LVGL

    // 把两个缓冲区交给 LVGL
    lv_disp_draw_buf_init(&draw_buf, draw_buf_1, draw_buf_2, LCD_WIDTH * 28);

    // 注册屏幕驱动
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = LCD_WIDTH;   // 水平分辨率 240
    disp_drv.ver_res  = LCD_HEIGHT;  // 垂直分辨率 280
    disp_drv.flush_cb = my_disp_flush; // 刷新回调
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

}

