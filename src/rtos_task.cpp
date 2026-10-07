#include "rtos_task.h"
#include "esp_timer.h"

// TaskUI 上一次喂给 LVGL 的时间点（微秒），用于计算真实流逝的时间
static int64_t ui_last_tick_us = 0;

struct tm timeinfo;

String WeatherType = "晴天";
String Temperature = "26°C";

SemaphoreHandle_t TimeMutex;

static String normalizeWeatherType(String raw)
{
    if (raw.indexOf("多云") >= 0) return "多云";
    if (raw.indexOf("冰雹") >= 0 || raw.indexOf("雹") >= 0) return "冰雹";
    if (raw.indexOf("雨夹雪") >= 0) return "雨夹雪";

    if (raw.indexOf("雷阵雨") >= 0)
    {
        if (raw.indexOf("强") >= 0 || raw.indexOf("大") >= 0) return "大雷阵雨";
        if (raw.indexOf("小") >= 0) return "小雷阵雨";
        return "雷阵雨";
    }

    if (raw.indexOf("暴雪") >= 0) return "大雪";
    if (raw.indexOf("大雪") >= 0) return "大雪";
    if (raw.indexOf("中雪") >= 0) return "中雪";
    if (raw.indexOf("小雪") >= 0) return "小雪";

    if (raw.indexOf("暴雨") >= 0) return "暴雨";
    if (raw.indexOf("大雨") >= 0) return "暴雨";
    if (raw.indexOf("中雨") >= 0) return "中雨";
    if (raw.indexOf("小雨") >= 0) return "小雨";

    if (raw.indexOf("晴") >= 0) return "晴";
    return "晴";
}

void rtos_init(void)
{
    TimeMutex = xSemaphoreCreateMutex();

    xTaskCreate(TaskClock, "Clock", 1024, NULL, 4, NULL);
    xTaskCreate(TaskUI, "UI", 4096, NULL, 1, NULL);
    xTaskCreate(Taskweather, "weather", 4096, NULL, 5, NULL);
    //xTaskCreate(TaskWifi, "Wifi", 4096, NULL, 3, NULL);
}

void TaskUI(void *pvParameters)
{
    while (1)
    {
        // 用“真实流逝的时间”喂 LVGL：原来固定写 lv_tick_inc(16)，
        // 只要 lv_timer_handler() 本身耗时超过 16ms（刷屏 SPI 传输就是这么慢），
        // LVGL 的内部时间就会比真实时间走得慢，动画变卡、触摸采样周期被拉长 → 触控不灵敏
        int64_t now_us = esp_timer_get_time();
        uint32_t elapsed_ms = (uint32_t)((now_us - ui_last_tick_us) / 1000);
        ui_last_tick_us = now_us;

        if (elapsed_ms > 0)
        {
            lv_tick_inc(elapsed_ms);
        }

        lv_timer_handler(); // LVGL 处理刷新、动画和触摸输入
        
        vTaskDelay(pdMS_TO_TICKS(2)); // 只让出 CPU，刷新节奏交给 LVGL 自己的定时器
    }
}

/*void TaskWifi(void *pvParameters)
{
    while (1)
    {
        handleWifiConfig();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}*/

void TaskClock(void *pvParameters)
{
    while (1)
    {
        // 不能用 getLocalTime(&timeinfo)：它默认超时是 5000ms，
        // 而时间又没有被 NTP 校准（main.cpp 里 configTime 是注释掉的），
        // 于是它会一直等到超时；更糟的是原来是在“持锁”状态下等的，
        // clock_update() 里的 xSemaphoreTake 就会把 LVGL 线程卡住最多 5 秒
        // → 表现就是滑动之后动画停住、触摸要过一会儿才恢复
        time_t now = time(NULL);
        struct tm local;
        localtime_r(&now, &local);

        xSemaphoreTake(TimeMutex, portMAX_DELAY);
        timeinfo = local;
        xSemaphoreGive(TimeMutex);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Taskweather(void *pvParameters)
{
    while (1)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;
            http.setTimeout(3000);
            http.begin("http://t.weather.itboy.net/api/weather/city/101101101");
            int httpCode = http.GET();

            if (httpCode == 200)
            {
                String payload = http.getString();

                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, payload);

                if(!error)
                {
                    String newType = normalizeWeatherType(doc["data"]["forecast"][0]["type"].as<String>());
                    //字包含晴 多 云 大 小 雷 阵 雨 冰 雹 夹 雪 暴 中
                    int temp = (int)(doc["data"]["wendu"].as<float>() + 0.5f);  // 四舍五入
                    String newTemp = String(temp) + "°C";
                    
                    WeatherType = newType;
                    Temperature = newTemp;
                }
            }
            http.end();
        }

        vTaskDelay(pdMS_TO_TICKS(6000000)); // 10分钟更新一次
    }
}


