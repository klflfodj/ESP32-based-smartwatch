#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_task_wdt.h"

#include "UI.h"
//#include "comms/wifi/WiFie.h"
//#include "comms/ble/ble.h"

extern struct tm timeinfo;
extern String WeatherType;
extern String Temperature;

extern SemaphoreHandle_t TimeMutex;


void rtos_init(void);
void TaskClock(void *pvParameters);
void TaskUI(void *pvParameters);
void Taskweather(void *pvParameters);
void TaskWifi(void *pvParameters);


#endif
