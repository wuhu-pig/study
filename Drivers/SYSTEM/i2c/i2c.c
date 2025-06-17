#include "i2c.h"
#include "delay.h"

#define I2C_DIRECTION_TRANSMITTER  0x00
#define I2C_DIRECTION_RECEIVER     0x01
#define I2C_TIMEOUT                10000

/***************************************************************
 * 内部函数：等待 I2C 标志位
 ***************************************************************/
static uint8_t I2C2_WaitFlag(volatile uint32_t* reg, uint32_t flag, uint8_t set) {
    uint32_t timeout = I2C_TIMEOUT;
    if (set) {
        while (((*reg) & flag) == 0) {
            if (--timeout == 0) return 0;
        }
    } else {
        while (((*reg) & flag) != 0) {
            if (--timeout == 0) return 0;
        }
    }
    return 1;
}

/***************************************************************
 * 生成 START 信号并发送地址
 ***************************************************************/
void I2C2_Start(uint8_t address, uint8_t direction) {
    if (!I2C2_WaitFlag(&I2C2->SR2, I2C_SR2_BUSY, 0)) return;

    I2C2->CR1 |= I2C_CR1_START;
    if (!I2C2_WaitFlag(&I2C2->SR1, I2C_SR1_SB, 1)) return;

    (void)I2C2->SR1; // 清除SB
    I2C2->DR = (address << 1) | direction;

    if (!I2C2_WaitFlag(&I2C2->SR1, I2C_SR1_ADDR, 1)) return;
    (void)I2C2->SR1;
    (void)I2C2->SR2;
}

/***************************************************************
 * 写入一个字节数据
 ***************************************************************/
void I2C2_Write(uint8_t data) {
    if (!I2C2_WaitFlag(&I2C2->SR1, I2C_SR1_TXE, 1)) return;
    I2C2->DR = data;
    if (!I2C2_WaitFlag(&I2C2->SR1, I2C_SR1_BTF, 1)) return;
}

/***************************************************************
 * 读取一个字节并发送 ACK
 ***************************************************************/
uint8_t I2C2_Read_ACK(void) {
    I2C2->CR1 |= I2C_CR1_ACK;
    if (!I2C2_WaitFlag(&I2C2->SR1, I2C_SR1_RXNE, 1)) return 0xFF;
    return I2C2->DR;
}

/***************************************************************
 * 读取一个字节并发送 NACK + STOP
 ***************************************************************/
uint8_t I2C2_Read_NACK(void) {
    uint8_t data;
    I2C2->CR1 &= ~I2C_CR1_ACK;
    if (!I2C2_WaitFlag(&I2C2->SR1, I2C_SR1_RXNE, 1)) return 0xFF;
    data = I2C2->DR;
    I2C2->CR1 |= I2C_CR1_STOP;
    return data;
}

/***************************************************************
 * 发送 STOP 信号
 ***************************************************************/
void I2C2_Stop(void) {
    I2C2->CR1 |= I2C_CR1_STOP;
    delay_us(10); // 给从机一点停止处理时间
}

/***************************************************************
 * I2C2 初始化（使用 PF0=SDA，PF1=SCL）
 ***************************************************************/
void AS5600_Init(void) {
    // 开启时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    // 设置 PF0 和 PF1 为复用功能 AF4（I2C2）
    GPIOF->MODER   |= (2 << (0 * 2)) | (2 << (1 * 2));  // 复用功能
    GPIOF->OTYPER  |= (1 << 0) | (1 << 1);              // 开漏输出
    GPIOF->OSPEEDR |= (3 << (0 * 2)) | (3 << (1 * 2));  // 高速
    GPIOF->PUPDR   |= (1 << (0 * 2)) | (1 << (1 * 2));  // 上拉
    GPIOF->AFR[0]  |= (4 << (0 * 4)) | (4 << (1 * 4));  // AF4 = I2C2

    // I2C 设置
    I2C2->CR1 &= ~I2C_CR1_PE;   // 禁止 I2C
    I2C2->CR2 = 42;             // 外设时钟（MHz）与你主频一致
    I2C2->CCR = 210;            // 标准模式（100kHz）
    I2C2->TRISE = 43;           // 最大上升时间
    I2C2->CR1 |= I2C_CR1_PE;    // 使能 I2C
}

/***************************************************************
 * 读取 AS5600 原始角度（返回 0~4095）
 ***************************************************************/
uint16_t AS5600_ReadRawAngle(void) {
    uint8_t high, low;
    uint16_t angle;

    // 第一步：写寄存器地址
    I2C2_Start(AS5600_I2C_ADDR, I2C_DIRECTION_TRANSMITTER);
    I2C2_Write(AS5600_ANGLE_REGISTER);
    I2C2_Stop();
    delay_us(10);  // 给 AS5600 处理时间

    // 第二步：读取高低字节
    I2C2_Start(AS5600_I2C_ADDR, I2C_DIRECTION_RECEIVER);
    high = I2C2_Read_ACK();
    low  = I2C2_Read_NACK();

    if (high == 0xFF && low == 0xFF) return 0xFFFF;  // 通信失败
    angle = ((uint16_t)high << 8) | low;
    return angle & 0x0FFF;  // 保留低 12 位
}

/***************************************************************
 * 获取角度（转换为 0~360.0°）
 ***************************************************************/
float AS5600_ReadAngle_Degrees(void) {
    uint16_t raw = AS5600_ReadRawAngle();
    if (raw == 0xFFFF) return -1.0f;  // 错误标志
    return (float)raw * 360.0f / 4096.0f;
}

/***************************************************************
 * 获取角度（转换为 -π~π）
 ***************************************************************/
float AS5600_ReadAngle_Radians(void) {
    uint16_t raw = AS5600_ReadRawAngle();
    if (raw == 0xFFFF) return -10.0f;  // 错误标志
    float radians = ((float)raw / 4096.0f) * (2.0f * 3.1415926f);
    if (radians > 3.1415926f)
        radians -= 2.0f * 3.1415926f;
    return radians;
}


#define OLED_I2C_ADDR 0x3C  // SSD1306 7位地址（0x3C或0x3D，视具体硬件）

// I2C1 初始化（PB6=SCL, PB7=SDA）
void I2C1_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // 使能GPIOB时钟
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;   // 使能I2C1时钟

    // 配置PB6和PB7为复用功能 AF4 (I2C1)
    GPIOB->MODER &= ~((3 << (6 * 2)) | (3 << (7 * 2)));
    GPIOB->MODER |= (2 << (6 * 2)) | (2 << (7 * 2));   // 复用模式
    GPIOB->OTYPER |= (1 << 6) | (1 << 7);              // 开漏输出
    GPIOB->OSPEEDR |= (3 << (6 * 2)) | (3 << (7 * 2)); // 高速
    GPIOB->PUPDR |= (1 << (6 * 2)) | (1 << (7 * 2));   // 上拉
    GPIOB->AFR[0] &= ~((0xF << (6 * 4)) | (0xF << (7 * 4)));
    GPIOB->AFR[0] |= (4 << (6 * 4)) | (4 << (7 * 4));  // AF4 = I2C1

    I2C1->CR1 &= ~I2C_CR1_PE;        // 关闭I2C1
    I2C1->CR2 = 42;                  // APB1时钟42MHz
    I2C1->CCR = 210;                 // 标准模式 100kHz (42MHz/210/2=100kHz)
    I2C1->TRISE = 43;                // TRISE = freq + 1
    I2C1->CR1 |= I2C_CR1_PE;         // 使能I2C1
}

// 等待标志位函数，set=1等待标志位置位，set=0等待标志位清零
static uint8_t I2C1_WaitFlag(volatile uint32_t* reg, uint32_t flag, uint8_t set) {
    uint32_t timeout = 10000;
    if (set) {
        while (((*reg) & flag) == 0) {
            if (--timeout == 0) return 0;
        }
    } else {
        while (((*reg) & flag) != 0) {
            if (--timeout == 0) return 0;
        }
    }
    return 1;
}

// I2C1 发送起始信号并发送设备地址+读写位
uint8_t I2C1_Start(uint8_t address, uint8_t direction) {
    if (!I2C1_WaitFlag(&I2C1->SR2, I2C_SR2_BUSY, 0)) return 0;  // 等待总线空闲

    I2C1->CR1 |= I2C_CR1_START;                                // 发送START
    if (!I2C1_WaitFlag(&I2C1->SR1, I2C_SR1_SB, 1)) return 0;  // 等待SB置位

    (void)I2C1->SR1;                                          // 读SR1清除SB标志
    I2C1->DR = (address << 1) | direction;                    // 发送地址

    if (!I2C1_WaitFlag(&I2C1->SR1, I2C_SR1_ADDR, 1)) return 0; // 等待ADDR置位
    (void)I2C1->SR1;                                          // 读SR1清除ADDR标志
    (void)I2C1->SR2;                                          // 读SR2完成地址阶段
    return 1;
}

// I2C1 发送数据字节
uint8_t I2C1_Write(uint8_t data) {
    if (!I2C1_WaitFlag(&I2C1->SR1, I2C_SR1_TXE, 1)) return 0;  // 等待TXE置位
    I2C1->DR = data;                                          // 发送数据
    if (!I2C1_WaitFlag(&I2C1->SR1, I2C_SR1_BTF, 1)) return 0; // 等待BTF置位
    return 1;
}

// I2C1 发送停止信号
void I2C1_Stop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
    delay_us(10);
}

// OLED 写命令
void OLED_WriteCommand(uint8_t cmd) {
    I2C1_Start(OLED_I2C_ADDR, 0);       // 写模式
    I2C1_Write(0x00);                   // 控制字节 0x00 表示命令
    I2C1_Write(cmd);
    I2C1_Stop();
}

// OLED 写数据
void OLED_WriteData(uint8_t data) {
    I2C1_Start(OLED_I2C_ADDR, 0);       // 写模式
    I2C1_Write(0x40);                   // 控制字节 0x40 表示数据
    I2C1_Write(data);
    I2C1_Stop();
}

// OLED 初始化（SSD1306示范）
void OLED_Init(void) {
    delay_ms(100);      // 上电延时
    OLED_WriteCommand(0xAE); // 关闭显示
    OLED_WriteCommand(0x20); // 设置内存地址模式
    OLED_WriteCommand(0x10); // 页地址模式
    OLED_WriteCommand(0xB0); // 设置页起始地址
    OLED_WriteCommand(0xC8); // COM输出扫描方向反转
    OLED_WriteCommand(0x00); // 设置低列地址
    OLED_WriteCommand(0x10); // 设置高列地址
    OLED_WriteCommand(0x40); // 设置起始行地址
    OLED_WriteCommand(0x81); // 设置对比度
    OLED_WriteCommand(0xFF); // 最大对比度
    OLED_WriteCommand(0xA1); // 段重映射
    OLED_WriteCommand(0xA6); // 正常显示
    OLED_WriteCommand(0xA8); // 多路复用比率
    OLED_WriteCommand(0x3F); // 1/64
    OLED_WriteCommand(0xA4); // 输出不跟随RAM内容
    OLED_WriteCommand(0xD3); // 设置显示偏移
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0xD5); // 设置时钟分频
    OLED_WriteCommand(0xF0);
    OLED_WriteCommand(0xD9); // 设置预充电周期
    OLED_WriteCommand(0x22);
    OLED_WriteCommand(0xDA); // 设置COM引脚硬件配置
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0xDB); // 设置VCOMH电压倍率
    OLED_WriteCommand(0x20);
    OLED_WriteCommand(0x8D); // 充电泵设置
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0xAF); // 打开显示
}

// OLED 清屏
void OLED_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_WriteCommand(0xB0 + page); // 设置页地址
        OLED_WriteCommand(0x00);        // 设置低列地址
        OLED_WriteCommand(0x10);        // 设置高列地址
        for (uint8_t col = 0; col < 128; col++) {
            OLED_WriteData(0x00);       // 清零数据
        }
    }
}

// 简单显示函数：在OLED第0页写一个字节（演示）
void OLED_ShowByte(uint8_t page, uint8_t col, uint8_t data) {
    OLED_WriteCommand(0xB0 + page);       // 页地址
    OLED_WriteCommand(0x00 + (col & 0x0F));    // 低列地址
    OLED_WriteCommand(0x10 + ((col >> 4) & 0x0F));  // 高列地址
    OLED_WriteData(data);
}
