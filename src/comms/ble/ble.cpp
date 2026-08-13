#include "ble.h"

NimBLECharacteristic *pTxCharacteristic = nullptr;
bool deviceConnected = false;

class BLECallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0)
        {
            Serial.println("Received value: ");
            for (int i = 0; i < rxValue.length(); i++)
            {
                Serial.print(rxValue[i]);
            }
            Serial.println();

            if (rxValue.find("wifi") != std::string::npos)
            {
                sendToPhone("正在连接WiFi...");
            }
            else
            {
                sendToPhone("未知指令");
            }

        }
    }
};

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

void sendToPhone(String message)
{
    if (deviceConnected && pTxCharacteristic != nullptr)
    {
        pTxCharacteristic->setValue(message.c_str());
        pTxCharacteristic->notify();
        Serial.println("Sent message to phone: " + message);
    }
}

void ble_init()
{
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
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->enableScanResponse(true);
    NimBLEDevice::startAdvertising();

    Serial.println("蓝牙已开启，等待手机连接...");
}

