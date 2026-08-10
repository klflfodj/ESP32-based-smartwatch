#include "UI.h"

// 卡片背景色，月牙挖空也用它
#define CARD_BG_COLOR 0x12161A

// 页面对象
static lv_obj_t *page_main;
static lv_obj_t *page_settings;

// 表盘控件，以后接 WiFi/天气代码时直接改这些
static lv_obj_t *time_label;
static lv_obj_t *date_year_label;
static lv_obj_t *date_month_label;
static lv_obj_t *date_day_label;
static lv_obj_t *battery_label;
static lv_obj_t *weather_label;
static lv_obj_t *temp_label;

// 每秒更新时间
static void clock_update(lv_timer_t *timer)
{
    uint32_t seconds = millis() / 1000;
    uint8_t h = (seconds / 3600) % 24;
    uint8_t m = (seconds / 60) % 60;

    lv_label_set_text_fmt(time_label, "%02d:%02d", h, m);
}

// 主页面：上滑进入设置页
static void main_swipe_cb(lv_event_t *e)
{
    if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP)
    {
        lv_scr_load_anim(page_settings, LV_SCR_LOAD_ANIM_MOVE_TOP, 125, 0, false);
    }
}

// 设置页：下滑返回主页面
static void settings_swipe_cb(lv_event_t *e)
{
    if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM)
    {
        lv_scr_load_anim(page_main, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 125, 0, false);
    }
}

// 装饰形状的公共样式：圆形、填色、不响应触摸
static void set_decoration_style(lv_obj_t *obj, lv_color_t color)
{
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, color, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
}

// 月牙占位：白色圆 + 背景色圆挖掉一角
static void create_moon_icon(void)
{
    lv_obj_t *moon = lv_obj_create(page_main);
    lv_obj_set_pos(moon, 158, 38);
    lv_obj_set_size(moon, 16, 16);
    set_decoration_style(moon, lv_color_hex(0xE8E8E8));

    lv_obj_t *cut = lv_obj_create(page_main);
    lv_obj_set_pos(cut, 163, 36);
    lv_obj_set_size(cut, 16, 16);
    set_decoration_style(cut, lv_color_hex(CARD_BG_COLOR));
}

// 云朵占位：三个圆 + 底部圆角矩形拼出云轮廓
static void create_cloud_icon(void)
{
    lv_color_t cloud_color = lv_color_hex(0xD0D0D0);

    lv_obj_t *c1 = lv_obj_create(page_main);
    lv_obj_set_pos(c1, 140, 176);
    lv_obj_set_size(c1, 10, 10);
    set_decoration_style(c1, cloud_color);

    lv_obj_t *c2 = lv_obj_create(page_main);
    lv_obj_set_pos(c2, 147, 170);
    lv_obj_set_size(c2, 12, 12);
    set_decoration_style(c2, cloud_color);

    lv_obj_t *c3 = lv_obj_create(page_main);
    lv_obj_set_pos(c3, 157, 176);
    lv_obj_set_size(c3, 10, 10);
    set_decoration_style(c3, cloud_color);

    lv_obj_t *base = lv_obj_create(page_main);
    lv_obj_set_pos(base, 140, 178);
    lv_obj_set_size(base, 26, 10);
    lv_obj_clear_flag(base, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(base, cloud_color, 0);
    lv_obj_set_style_bg_opa(base, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(base, 4, 0);
}

// 电池框：外框 + 电量文字 + 电池头
static void create_battery(void)
{
    lv_obj_t *battery = lv_obj_create(page_main);
    lv_obj_set_pos(battery, 112, 96);
    lv_obj_set_size(battery, 44, 22);
    lv_obj_clear_flag(battery, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(battery, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(battery, 1, 0);
    lv_obj_set_style_border_color(battery, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_radius(battery, 3, 0);

    battery_label = lv_label_create(battery);
    lv_obj_center(battery_label);
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(battery_label, "80%");

    // 电池头：右边的小凸起
    lv_obj_t *tip = lv_obj_create(page_main);
    lv_obj_set_pos(tip, 157, 101);
    lv_obj_set_size(tip, 4, 8);
    lv_obj_clear_flag(tip, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(tip, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(tip, 1, 0);
    lv_obj_set_style_border_color(tip, lv_color_hex(0xCCCCCC), 0);
}

// 创建主页面
static void ui_create_main_page(void)
{
    page_main = lv_obj_create(NULL);
    lv_obj_add_flag(page_main, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(page_main, lv_color_hex(CARD_BG_COLOR), 0);

    // 左上大号时间
    time_label = lv_label_create(page_main);
    lv_obj_set_pos(time_label, 22, 28);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(time_label, "12:11");

    create_moon_icon();

    // 日期三行：中文需要以后换自定义中文字体
    date_year_label = lv_label_create(page_main);
    lv_obj_set_pos(date_year_label, 24, 78);
    lv_obj_set_style_text_font(date_year_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(date_year_label, lv_color_hex(0xE8E8E8), 0);
    lv_label_set_text(date_year_label, "2026");

    date_month_label = lv_label_create(page_main);
    lv_obj_set_pos(date_month_label, 24, 108);
    lv_obj_set_style_text_font(date_month_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(date_month_label, lv_color_hex(0xE8E8E8), 0);
    lv_label_set_text(date_month_label, "8月");// 转好中文字体后换 font_cn

    date_day_label = lv_label_create(page_main);
    lv_obj_set_pos(date_day_label, 24, 138);
    lv_obj_set_style_text_font(date_day_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(date_day_label, lv_color_hex(0xE8E8E8), 0);
    lv_label_set_text(date_day_label, "10日");// 转好中文字体后换 font_cn

    create_battery();

    // 天气文字
    weather_label = lv_label_create(page_main);
    lv_obj_set_pos(weather_label, 24, 178);
    lv_obj_set_style_text_font(weather_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(weather_label, lv_color_hex(0xE8E8E8), 0);
    lv_label_set_text(weather_label, "晴天");// 转好中文字体后换 font_cn

    temp_label = lv_label_create(page_main);
    lv_obj_set_pos(temp_label, 88, 178);
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0xE8E8E8), 0);
    lv_label_set_text(temp_label, "26C");

    create_cloud_icon();

    // 上滑切换设置页
    lv_obj_add_event_cb(page_main, main_swipe_cb, LV_EVENT_GESTURE, NULL);
}

// 创建设置页（保持下滑返回）
static void ui_create_settings_page(void)
{
    page_settings = lv_obj_create(NULL);
    lv_obj_add_flag(page_settings, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(page_settings, lv_color_hex(0x101010), 0);

    lv_obj_t *title = lv_label_create(page_settings);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(title, "Settings");

    lv_obj_t *hint = lv_label_create(page_settings);
    lv_obj_align(hint, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x888888), 0);
    lv_label_set_text(hint, "Swipe down to go back");

    lv_obj_add_event_cb(page_settings, settings_swipe_cb, LV_EVENT_GESTURE, NULL);
}

// 对外接口：创建所有页面并显示表盘
void UI_init(void)
{
    ui_create_main_page();
    ui_create_settings_page();

    lv_timer_create(clock_update, 1000, NULL);

    lv_scr_load(page_main);
}