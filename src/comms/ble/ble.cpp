#include "ble.h"

SemaphoreHandle_t wifiConfigMutex = nullptr;
NimBLECharacteristic *pTxCharacteristic = nullptr;// 发送特征值对象，用于发送数据到手机
bool deviceConnected = false;// 是否已连接到手机

String wifiSsid = "";
String wifiPassword = "";
volatile bool wifiReadyToConnect = false;

// -----------------------
// 功能介绍：
// 处理手机发送的指令
// -----------------------
class BLECallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        String command = pCharacteristic->getValue().c_str();
        command.trim();

        if (command == "wifi")
        {
            wifiSsid = "";
            wifiPassword = "";
            sendToPhone("name:");
        }
        else if (command.startsWith("name:"))
        {
            wifiSsid = command.substring(5);
            wifiSsid.trim();
            sendToPhone("password:");
        }
        else if (command.startsWith("pass:"))
        {
            if (wifiConfigMutex != nullptr) xSemaphoreTake(wifiConfigMutex, portMAX_DELAY);

            wifiPassword = command.substring(5);
            wifiPassword.trim();

            if (wifiSsid.length() == 0 || wifiPassword.length() == 0)
            {
                if (wifiConfigMutex != nullptr) xSemaphoreGive(wifiConfigMutex);
                sendToPhone("配置错误");
                return;
            }

            WiFie_SaveCredentials(wifiSsid, wifiPassword);
            wifiReadyToConnect = true;

            if (wifiConfigMutex != nullptr) xSemaphoreGive(wifiConfigMutex);
            sendToPhone("正在连接WiFi...");
        }
        else
        {
            sendToPhone("未知指令");
        }
    }
};

// -----------------------
// 功能介绍：
// 处理手机连接和断开
// -----------------------
class MyServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *server, NimBLEConnInfo &connInfo) override
    {
        deviceConnected = true;
        Serial.println("BLE connected");
    }

    void onDisconnect(NimBLEServer *server, NimBLEConnInfo &connInfo, int reason) override
    {
        deviceConnected = false;
        Serial.println("BLE disconnected");
        NimBLEDevice::startAdvertising();
    }
};

// -----------------------
// 功能介绍：
// 向手机发送消息
// -----------------------
// message → 要发送的消息
void sendToPhone(String message)
{
    if (deviceConnected && pTxCharacteristic != nullptr)
    {
        pTxCharacteristic->setValue(message.c_str());
        pTxCharacteristic->notify();
        Serial.println("Sent message to phone: " + message);
    }
}

// 处理WiFi配置
void handleWifiConfig()
{
    if (!wifiReadyToConnect)
    {
        return;
    }

    String ssid;
    String pass;

    if (wifiConfigMutex != nullptr) xSemaphoreTake(wifiConfigMutex, portMAX_DELAY);

    if (!wifiReadyToConnect)
    {
        if (wifiConfigMutex != nullptr) xSemaphoreGive(wifiConfigMutex);
        return;
    }

    wifiReadyToConnect = false;
    ssid = wifiSsid;
    pass = wifiPassword;

    if (wifiConfigMutex != nullptr) xSemaphoreGive(wifiConfigMutex);

    bool ok = WiFie_Init(ssid.c_str(), pass.c_str());
    sendToPhone(ok ? "WIFI_CONNECTED" : "WIFI_FAILED");
}

void ble_init()
{
    if (wifiConfigMutex == nullptr)
    {
        wifiConfigMutex = xSemaphoreCreateMutex();
    }

    NimBLEDevice::init("MyWatch");

    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());


    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    pTxCharacteristic = pService->createCharacteristic
    (TX_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::NOTIFY);

    NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic
    (RX_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);
    pRxCharacteristic->setCallbacks(new BLECallbacks());

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName("MyWatch");
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->enableScanResponse(true);
    NimBLEDevice::startAdvertising();

    Serial.println("蓝牙已开启，等待手机连接...");
}

