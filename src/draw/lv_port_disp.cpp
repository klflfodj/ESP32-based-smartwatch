#include "lv_port_disp.h"

#define LCD_HEIGHT_T LCD_HEIGHT / 3

static lv_color_t *draw_buf_1;


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

    draw_buf_1 = (lv_color_t *)heap_caps_malloc
    (LCD_WIDTH * LCD_HEIGHT_T * sizeof(lv_color_t),MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    if (draw_buf_1 == NULL) 
    {
        Serial.println("draw buffer alloc failed");
        return;
    }

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
    // 抬手时 LVGL 仍然需要一组有效坐标，这里保留最后一次按下的位置
    static uint16_t last_x = 0;
    static uint16_t last_y = 0;
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

        // 噪声数据可能导致坐标越界，钳位到屏幕范围内，避免指针被甩到屏幕外
        if (x > LCD_WIDTH - 1) x = LCD_WIDTH - 1;
        if (y > LCD_HEIGHT - 1) y = LCD_HEIGHT - 1;

        last_x = x;
        last_y = y;

        // 触摸屏有触摸点
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        // 触摸屏没有触摸点
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->point.x = last_x;
    data->point.y = last_y;
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
    // 默认要滑动 50 像素才算手势，调小后轻扫也能识别，翻页更灵敏
    indev_drv.gesture_limit = 30;
    indev_drv.gesture_min_velocity = 2;
    lv_indev_drv_register(&indev_drv);// 注册输入设备驱动
}

