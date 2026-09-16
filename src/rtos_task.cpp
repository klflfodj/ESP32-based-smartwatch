#include "rtos_task.h"

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
    xTaskCreate(TaskWifi, "Wifi", 4096, NULL, 3, NULL);
}

void TaskUI(void *pvParameters)
{
    while (1)
    {
        lv_tick_inc(5);     // 每 5ms 告诉 LVGL 时间过了多久
        lv_timer_handler(); // LVGL 处理刷新和动画
        
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void TaskWifi(void *pvParameters)
{
    while (1)
    {
        handleWifiConfig();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void TaskClock(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(TimeMutex,portMAX_DELAY);
        getLocalTime(&timeinfo);
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


