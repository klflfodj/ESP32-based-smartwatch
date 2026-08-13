#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include "comms/wifi/WiFie.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define TX_CHARACTERISTIC_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define RX_CHARACTERISTIC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

extern NimBLECharacteristic *pTxCharacteristic;
extern bool deviceConnected;

extern String wifiSsid;
extern String wifiPassword;
extern volatile bool wifiReadyToConnect;

void ble_init();
void sendToPhone(String message);
void handleWifiConfig();


#endif
