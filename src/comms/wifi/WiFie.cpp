#include "WiFie.h"

// -----------------------
// WiFie 初始化函数
// -----------------------
// 任务功能：初始化 WiFi 连接，等待连接成功或超时
// -----------------------
// 连接时间超过 10 秒后，打印 WiFi 连接失败信息
// 连接成功后，打印 WiFi 连接成功信息
// -----------------------
bool WiFie_Init(const char* ssid, const char* password)
{
  WiFi.begin(ssid,password);

  int timeout = 0;

  while(WiFi.status()!=WL_CONNECTED && timeout<20)
  {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.println("WiFi Connected");
  }
  else
  {
    Serial.println();
    Serial.println("WiFi Connect Failed");
    WiFi.disconnect();
  }

  return WiFi.status() == WL_CONNECTED;
}

Preferences wifiPrefs;
// -----------------------
// 功能介绍：
// 保存 WiFi 连接凭证到偏好设置
// -----------------------
// ssid → 要保存的 SSID
// password → 要保存的密码
void WiFie_SaveCredentials(const String& ssid, const String& password)
{
    wifiPrefs.begin("wifi", false);
    wifiPrefs.putString("ssid", ssid);
    wifiPrefs.putString("pass", password);
    wifiPrefs.end();
}
// -----------------------
// 功能介绍：
// 从偏好设置中获取保存的 SSID
// -----------------------
// 返回值：保存的 SSID
String WiFie_GetSavedSsid()
{
    wifiPrefs.begin("wifi", true);
    String ssid = wifiPrefs.getString("ssid", "");
    wifiPrefs.end();
    return ssid;
}
// -----------------------
// 功能介绍：
// 从偏好设置中获取保存的密码
// -----------------------
// 返回值：保存的密码
String WiFie_GetSavedPassword()
{
    wifiPrefs.begin("wifi", true);
    String pass = wifiPrefs.getString("pass", "");
    wifiPrefs.end();
    return pass;
}

