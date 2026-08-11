#include "CST816S_Touch.h"

void Touch_Init(void)
{
    pinMode(TOUCH_RST, OUTPUT);
    pinMode(TOUCH_INT, INPUT);

    // RST 高 -> 低 -> 高 完成一次复位
    digitalWrite(TOUCH_RST, HIGH);
    delay(20);
    digitalWrite(TOUCH_RST, LOW);
    delay(20);
    digitalWrite(TOUCH_RST, HIGH);
    delay(50);

    Wire.begin(TOUCH_SDA, TOUCH_SCL, 400000);

}

bool Touch_GetXY(uint16_t *x, uint16_t *y)
{
    // 发送读取触摸坐标命令
    Wire.beginTransmission(CST816S_ADDR);
    Wire.write(0x02);
    Wire.endTransmission();

    Wire.requestFrom(CST816S_ADDR, 5);
    if (Wire.available() < 5)
    {
        return false;// 读取失败，没有数据返回
    }
    
    // 读取触摸坐标
    uint8_t point = Wire.read();
    uint8_t xH = Wire.read();
    uint8_t xL = Wire.read();
    uint8_t yH = Wire.read();
    uint8_t yL = Wire.read();

    // 检查是否有触摸点
    if (point == 0)
    {
        return false;// 没有触摸点
    }

    // 只取低 4 位，防止高位夹带标志位或噪声
    *x = ((xH & 0x0F) << 8) | xL;
    *y = ((yH & 0x0F) << 8) | yL;

    return true;
}

