#include "rtos_task.h"

struct tm timeinfo;
SemaphoreHandle_t TimeMutex;

void rtos_init(void)
{
    TimeMutex = xSemaphoreCreateMutex();

    xTaskCreate(TaskClock, "Clock", 2048, NULL, 5, NULL);
    xTaskCreate(TaskUI, "UI", 4096, NULL, 1, NULL);
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


