#include "lv_port_disp.h"

#include "ST7789.h"
#include "CST816S_Touch.h"

#define LCD_HEIGHT_T LCD_HEIGHT / 3

// 一个 1/3 屏缓冲，双缓冲交替使用，内存约 33 KB
static lv_color_t draw_buf_1[LCD_WIDTH * LCD_HEIGHT_T];


// 整屏缓冲指针，运行时用 malloc 从堆里申请
static lv_color_t *draw_buf_full;

static lv_disp_draw_buf_t draw_buf;// 绘制缓冲区，用于存储需要刷新的矩形区域
static lv_disp_drv_t disp_drv;// 显示驱动对象，用于刷新屏幕
static lv_indev_drv_t indev_drv;// 输入设备对象，用于读触摸屏数据

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

    // 设置窗口，然后把整块像素一次性发送
    ST7789_SetAddressWindow(x1, y1, x2, y2);
    SPI_WriteDataBuffer(&LCD_SPI_Config,  (const uint8_t *)color_p, pixel_count * 2);

    // 通知 LVGL 刷新完成
    lv_disp_flush_ready(disp_drv);
}

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

    // 一个 1/4 屏缓冲，每次刷 1/4 屏，分块数从 10 块到 5 块
    lv_disp_draw_buf_init(&draw_buf, draw_buf_1, NULL, LCD_WIDTH * LCD_HEIGHT_T);

    // 注册屏幕驱动
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = LCD_WIDTH;   // 水平分辨率 240
    disp_drv.ver_res  = LCD_HEIGHT;  // 垂直分辨率 280
    disp_drv.flush_cb = my_disp_flush; // 刷新回调
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
}
// -----------------------
// 功能介绍：
// 读取 LV屏输入数据
// -----------------------
// drv → LVGL 输入设备对象
// data → LVGL 输入数据结构体
// -----------------------
void lv_indev_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x = 0;
    uint16_t y = 0;
    // 读取触摸屏坐标
    bool pressed = Touch_GetXY(&x, &y);

    if (pressed)
    {
        // 触摸 Y 轴和屏幕方向相反，翻转 Y 坐标
        y = LCD_HEIGHT - 1 - y;
        // 触摸 X 轴也和屏幕方向相反，翻转 X 坐标
        x = LCD_WIDTH - 1 - x;

        // 触摸屏有触摸点
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        // 触摸屏没有触摸点
        data->state = LV_INDEV_STATE_RELEASED;
    }

}

// -----------------------
// 功能介绍：
// 初始化 LV屏输入驱动
// -----------------------
// indev_drv → LVGL 输入设备对象
// -----------------------
void lv_touch_init(void)
{
    lv_indev_drv_init(&indev_drv);// 初始化输入设备驱动
    indev_drv.type = LV_INDEV_TYPE_POINTER;// 触摸类型
    indev_drv.read_cb = lv_indev_read;// 读取回调函数
    lv_indev_drv_register(&indev_drv);// 注册输入设备驱动
}

