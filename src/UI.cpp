#include "UI.h"
#include "misans_font.h"

// 页面对象
static lv_obj_t *page_main;
static lv_obj_t *page_settings;
static lv_obj_t *page_info;

// 时间显示控件
static lv_obj_t *time_label_hour;
static lv_obj_t *time_label_minute;
// 电池显示控件
static lv_obj_t *battery_arc;
// 星期显示控件
static lv_obj_t *weekday_label;
// 天气显示控件
static lv_obj_t *weather_label;
// 温度显示控件
static lv_obj_t *temperature_label;



// 每秒更新时间
static void clock_update(lv_timer_t *timer)
{
    uint32_t seconds = millis() / 1000;
    uint8_t h = (seconds / 3600) % 24;
    uint8_t m = (seconds / 60) % 60;

    h += 12;
    m += 23;

    lv_label_set_text_fmt(time_label_hour, "%02d", h);
    lv_label_set_text_fmt(time_label_minute, "%02d", m);

}

// 主页面：上滑进入信息页，下滑进入设置页
static void main_swipe_cb(lv_event_t *e)
{
    if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP)
    {
        lv_scr_load_anim(page_info, LV_SCR_LOAD_ANIM_MOVE_TOP, 100, 0, false);
    }
    else if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM)
    {
        lv_scr_load_anim(page_settings, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 100, 0, false);
    }
}

// 设置页：上滑返回主页面
static void settings_swipe_cb(lv_event_t *e)
{
    if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP)
    {
        lv_scr_load_anim(page_main, LV_SCR_LOAD_ANIM_MOVE_TOP, 100, 0, false);
    }
}

// 信息页：下滑返回主页面
static void info_swipe_cb(lv_event_t *e)
{
    if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM)
    {
        lv_scr_load_anim(page_main, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 100, 0, false);
    }
}

// 创建主页面
static void ui_create_main_page(void)
{
    //-----------------------------
    // 主页面
    //-----------------------------
    page_main = lv_obj_create(NULL);
    lv_obj_add_flag(page_main, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(page_main, lv_color_hex(0x101010), 0);

    lv_obj_set_style_radius(page_main, 0, 0);
    lv_obj_set_style_border_width(page_main, 0, 0);
    lv_obj_set_style_pad_all(page_main, 0, 0);
    
    // 上半部分（暖色矩形，覆盖左上区域）
    lv_obj_t *top_part = lv_obj_create(page_main);
    lv_obj_set_size(top_part, 380, 190);  // 宽高比约 2:1
    lv_obj_set_pos(top_part, -230, -30);  // 向左偏移，露出右下角
    lv_obj_set_style_bg_color(top_part, lv_color_hex(0xBDB76B), 0);
    lv_obj_set_style_radius(top_part, 0, 0);
    lv_obj_set_style_border_width(top_part, 0, 0);
    lv_obj_clear_flag(top_part, LV_OBJ_FLAG_SCROLLABLE);
    
    
    // 把两个背景移到最下层
    lv_obj_move_background(top_part);

    
    //-----------------------------
    // 时间
    //-----------------------------
    time_label_hour = lv_label_create(page_main);
    time_label_minute = lv_label_create(page_main);
    
    lv_obj_align(time_label_hour, LV_ALIGN_TOP_MID, -25, 20);
    lv_obj_align(time_label_minute, LV_ALIGN_TOP_MID, -45, 120);

    lv_obj_set_style_text_font(time_label_hour, &misans_time, 0);
    lv_obj_set_style_text_font(time_label_minute, &misans_time, 0);

    lv_obj_set_style_text_color(time_label_hour, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_color(time_label_minute, lv_color_hex(0xF0E68C), 0);

    lv_label_set_text(time_label_hour, "00");
    lv_label_set_text(time_label_minute, "00");
    //-----------------------------


    //-----------------------------
    // 电池
    battery_arc = lv_arc_create(page_main);

    lv_obj_set_size(battery_arc, 60, 60);
    lv_obj_align(battery_arc, LV_ALIGN_TOP_RIGHT, -15, 25);

    lv_obj_remove_style(battery_arc, NULL, LV_PART_KNOB); // 去掉中间的旋钮/滑块（表盘不需要拖动）

    // 设置角度范围：从 -90°（顶部）顺时针走完一圈
    lv_arc_set_bg_angles(battery_arc, 0, 360);       // 背景弧范围
    lv_arc_set_range(battery_arc, 0, 100);            // 值范围 0~100%
    lv_arc_set_value(battery_arc, 85);                // 初始值 85%

    // 背景弧样式（灰色底环）
    static lv_style_t style_arc_bg;
    lv_style_init(&style_arc_bg);
    lv_style_set_arc_width(&style_arc_bg, 8);         // 弧线粗细
    lv_style_set_arc_color(&style_arc_bg, lv_color_hex(0x333333));  // 暗灰色底
    lv_style_set_arc_rounded(&style_arc_bg, true);    // 圆头端点
    lv_obj_add_style(battery_arc, &style_arc_bg, LV_PART_MAIN);
    
    // 前景弧样式（电量指示色）
    static lv_style_t style_arc_indicator;
    lv_style_init(&style_arc_indicator);
    lv_style_set_arc_width(&style_arc_indicator, 8);
    lv_style_set_arc_color(&style_arc_indicator, lv_color_hex(0x00FF88));  // 绿色
    lv_style_set_arc_rounded(&style_arc_indicator, true);
    lv_obj_add_style(battery_arc, &style_arc_indicator, LV_PART_INDICATOR);
    
    // 可选：中间显示百分比数字
    lv_obj_t *percent_label = lv_label_create(battery_arc);
    lv_label_set_text_fmt(percent_label, "%d%%", 85);
    lv_obj_set_style_text_color(percent_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(percent_label, &lv_font_montserrat_20, 0);  // 用小字号
    lv_obj_center(percent_label);
    //-----------------------------

    //-----------------------------
    // 星期
    //-----------------------------
    weekday_label = lv_label_create(page_main);
    lv_obj_align(weekday_label, LV_ALIGN_BOTTOM_MID, -50, -25);
    lv_obj_set_style_text_color(weekday_label, lv_color_hex(0xC19A6B), 0);
    lv_obj_set_style_text_font(weekday_label, &misans_week, 0);
    lv_label_set_text(weekday_label, "星期二");  // 初始文本，后面会更新
    //-----------------------------

    //-----------------------------
    // 天气
    //-----------------------------
    weather_label = lv_label_create(page_main);
    lv_obj_align(weather_label, LV_ALIGN_BOTTOM_MID, 75, -130);
    lv_obj_set_style_text_color(weather_label, lv_color_hex(0xEEDC82), 0);
    lv_obj_set_style_text_font(weather_label, &misans_Weather, 0);
    lv_label_set_text(weather_label, "晴天\n  /");  // 初始文本，后面会更新
    //----------------------------- 
    
    //-----------------------------
    // 温度
    //-----------------------------
    temperature_label = lv_label_create(page_main);
    lv_obj_align(temperature_label, LV_ALIGN_BOTTOM_MID, 75, -100);
    lv_obj_set_style_text_color(temperature_label, lv_color_hex(0xEEDC82), 0);
    lv_obj_set_style_text_font(temperature_label, &misans_nem, 0);
    lv_label_set_text(temperature_label, "25°C");  // 初始文本，后面会更新
    //-----------------------------


    // 上滑切换信息页，下滑返回主页面
    lv_obj_add_event_cb(page_main, main_swipe_cb, LV_EVENT_GESTURE, NULL);
}

// 创建设置页（保持上滑返回主页面）
static void ui_create_settings_page(void)
{
    page_settings = lv_obj_create(NULL);
    lv_obj_add_flag(page_settings, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(page_settings, lv_color_hex(0x101010), 0);

    lv_obj_add_event_cb(page_settings, settings_swipe_cb, LV_EVENT_GESTURE, NULL);
}

// 创建信息页（保持下滑返回）
static void ui_create_info_page(void)
{
    page_info = lv_obj_create(NULL);
    lv_obj_add_flag(page_info, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(page_info, lv_color_hex(0x101010), 0);

    lv_obj_t *title = lv_label_create(page_info);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(title, "Settings");

    lv_obj_t *hint = lv_label_create(page_settings);
    lv_obj_align(hint, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x888888), 0);
    lv_label_set_text(hint, "Swipe down to go back");

    lv_obj_add_event_cb(page_info, info_swipe_cb, LV_EVENT_GESTURE, NULL);
}

// 对外接口：创建所有页面并显示表盘
void UI_init(void)
{
    ui_create_main_page();
    ui_create_info_page();
    ui_create_settings_page();

    lv_timer_create(clock_update, 1000, NULL);

    lv_scr_load(page_main);
}
