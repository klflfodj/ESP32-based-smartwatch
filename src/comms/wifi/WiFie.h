#ifndef __WIFIE_H
#define __WIFIE_H

#include <WiFi.h>
#include <Preferences.h>

bool WiFie_Init(const char* ssid, const char* password);
void WiFie_SaveCredentials(const String& ssid, const String& password);
String WiFie_GetSavedSsid();
String WiFie_GetSavedPassword();

#endif
