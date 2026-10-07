#include "CST816s_Touch.h"

// -----------------------
// CST816S 寄存器
// -----------------------
#define CST816S_REG_GESTURE       0x01// 手势 ID
#define CST816S_REG_TOUCH_NUM     0x02// 触摸点数量
#define CST816S_REG_XPOS_HIGH     0x03
#define CST816S_REG_XPOS_LOW      0x04
#define CST816S_REG_YPOS_HIGH     0x05
#define CST816S_REG_YPOS_LOW      0x06
#define CST816S_REG_IRQ_CTL       0xFA// 中断控制：使能触摸/变化上报
#define CST816S_REG_DIS_AUTOSLEEP 0xFE// 关闭自动休眠

// 中断控制值：EnMotion | EnChange | EnTouch
#define CST816S_IRQ_ENABLE_VALUE  0x70
// 关闭自动休眠：芯片睡下去后第一次触摸会被丢掉，表现就是“触摸不灵敏”
#define CST816S_DISABLE_AUTOSLEEP 1
// I2C 偶发 NACK 会丢点，失败时重试几次
#define CST816S_READ_RETRY        2

static void CST816S_WriteReg(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(CST816S_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

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

    // 复位后重新配置：部分批次的 CST816S 上电后只上报一次触摸，必须打开中断上报
    CST816S_WriteReg(CST816S_REG_IRQ_CTL, CST816S_IRQ_ENABLE_VALUE);

#if CST816S_DISABLE_AUTOSLEEP
    // 关掉自动休眠，保证第一次按下就能被读到
    CST816S_WriteReg(CST816S_REG_DIS_AUTOSLEEP, 0x01);
#endif

    delay(10);
}

bool Touch_GetXY(uint16_t *x, uint16_t *y)
{
    uint8_t buf[CST816S_REG_YPOS_LOW + 1] = {0};

    for (uint8_t retry = 0; retry <= CST816S_READ_RETRY; retry++)
    {
        // 从 0x00 开始一次性读出手势、触摸点数和坐标：
        // 一次 I2C 事务比分开读更稳，也不会出现寄存器读到一半又更新的情况
        Wire.beginTransmission(CST816S_ADDR);
        Wire.write((uint8_t)0x00);

        if (Wire.endTransmission(false) != 0)// false：重复起始，不释放总线
        {
            continue;// 发送失败，重试
        }

        if (Wire.requestFrom((uint16_t)CST816S_ADDR, (uint8_t)sizeof(buf)) != (uint8_t)sizeof(buf))
        {
            continue;// 读到的字节数不对，重试
        }

        for (uint8_t i = 0; i < sizeof(buf); i++)
        {
            buf[i] = Wire.read();
        }

        // 检查是否有触摸点
        if ((buf[CST816S_REG_TOUCH_NUM] & 0x0F) == 0)
        {
            return false;// 没有触摸点
        }

        // 只取低 4 位，防止高位夹带标志位或噪声
        *x = ((uint16_t)(buf[CST816S_REG_XPOS_HIGH] & 0x0F) << 8) | buf[CST816S_REG_XPOS_LOW];
        *y = ((uint16_t)(buf[CST816S_REG_YPOS_HIGH] & 0x0F) << 8) | buf[CST816S_REG_YPOS_LOW];

        return true;
    }

    return false;// 重试后仍然读不到
}
