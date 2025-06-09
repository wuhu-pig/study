#include "spi.h"


// SPI2 GPIO 配置为复用功能（从机）
void SPI2_GPIO_SlaveInit(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // PB12 = NSS
    GPIOB->MODER   &= ~(3 << (12 * 2));
    GPIOB->MODER   |=  (2 << (12 * 2));     // AF
    GPIOB->AFR[1]  &= ~(0xF << ((12 - 8) * 4));
    GPIOB->AFR[1]  |=  (5 << ((12 - 8) * 4)); // AF5
    GPIOB->OSPEEDR |=  (3 << (12 * 2));
    GPIOB->PUPDR   &= ~(3 << (12 * 2));     // 无上下拉

    // PB13 = SCK
    GPIOB->MODER   &= ~(3 << (13 * 2));
    GPIOB->MODER   |=  (2 << (13 * 2));     // AF
    GPIOB->AFR[1]  &= ~(0xF << ((13 - 8) * 4));
    GPIOB->AFR[1]  |=  (5 << ((13 - 8) * 4));
    GPIOB->OSPEEDR |=  (3 << (13 * 2));

    // PC3 = MOSI（主机发来，从机接收）
    GPIOC->MODER   &= ~(3 << (3 * 2));
    GPIOC->MODER   |=  (2 << (3 * 2));      // AF
    GPIOC->AFR[0]  &= ~(0xF << (3 * 4));
    GPIOC->AFR[0]  |=  (5 << (3 * 4));
    GPIOC->OSPEEDR |=  (3 << (3 * 2));

    // PC2 = MISO（主机接收，从机发送，默认不需要配置）
    GPIOC->MODER   &= ~(3 << (2 * 2));
    GPIOC->MODER   |=  (2 << (2 * 2));      // AF
    GPIOC->AFR[0]  &= ~(0xF << (2 * 4));
    GPIOC->AFR[0]  |=  (5 << (2 * 4));
    GPIOC->OSPEEDR |=  (3 << (2 * 2));
}

// SPI2 初始化为从机模式
void SPI2_SlaveInit(void) {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    SPI2->CR1 = 0;

    // CPOL = 0, CPHA = 0（模式0，主从必须一致）
    SPI2->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

    // 设置从机（默认MSTR=0）
    SPI2->CR1 &= ~SPI_CR1_MSTR;

    // 硬件NSS输入，不配置 SSM
    SPI2->CR1 &= ~(SPI_CR1_SSM | SPI_CR1_SSI);

    // 使能 SPI2
    SPI2->CR1 |= SPI_CR1_SPE;
}

// 接收 1 个字节（从 MOSI 线）
uint8_t SPI2_ReceiveByte(void) {
    while (!(SPI2->SR & SPI_SR_RXNE));  // 等待接收完成
    return SPI2->DR;
}

// SPI2 发送一个字节（等待 TXE 置位 -> 发送 -> 等待发送完成）
void SPI2_SendByte(uint8_t data) {
    while (!(SPI2->SR & SPI_SR_TXE));    // 等待TXE置位
    SPI2->DR = data;
    while (!(SPI2->SR & SPI_SR_TXE));    // 再次等待TXE置位（DR空）
    while (SPI2->SR & SPI_SR_BSY);       // 等待总线空闲
}
//此时如果主机发送 0x55，应该收到 0x55 回应
//如果主机使用的是模式1、2或3，那么从机会接收不到正确数据  确保主从两边 SPI 模式完全一致
//建议测试时用主机 MCU 或 FT232H/树莓派等设备确保 NSS 引脚连通并正确拉低（逻辑分析仪建议连接 PB12
void SPI2_Test(void)
{
				uint8_t recv = SPI2_ReceiveByte();  // 阻塞接收
        SPI2_SendByte(recv);                // 回发数据做loopback测试
}

