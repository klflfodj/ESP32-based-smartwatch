#include <Arduino.h>
#include "draw/LCD_GFX.h"
#include "drivers/SPI/ST7789.h"
#include "lvgl.h"
#include "draw/lv_port_disp.h"
#include "drivers/IIC/CST816s_Touch.h"
#include "comms/wifi/WiFie.h"
#include "comms/ble/ble.h"


#include "UI.h"

void setup() 
{
  Serial.begin(115200);
  ble_init();

  ST7789_Init();
  Touch_Init();
  lvgl_init();
  lv_touch_init();
  String savedSsid = WiFie_GetSavedSsid();
  String savedPass = WiFie_GetSavedPassword();
  if (!wifiReadyToConnect && savedSsid.length() > 0)
  {
    wifiSsid = savedSsid;
    wifiPassword = savedPass;
    wifiReadyToConnect = true;
  }
  configTime(8 * 3600,0,"ntp.aliyun.com");

  UI_init();

  /* 初始化 RTOS */
  delay(100);
  rtos_init();

}

void loop() 
{
  
}
