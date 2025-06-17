#include "spi.h"
#include "delay.h"

// SPI2: 主机 GPIO 初始化（SCK=MOSI=MISO）
void SPI2_GPIO_MasterInit(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // PB13 = SCK
    GPIOB->MODER &= ~(3 << (13 * 2));
    GPIOB->MODER |=  (2 << (13 * 2)); // AF
    GPIOB->AFR[1] &= ~(0xF << ((13 - 8) * 4));
    GPIOB->AFR[1] |=  (5 << ((13 - 8) * 4));
    GPIOB->OSPEEDR |= (3 << (13 * 2)); // 高速

    // PC3 = MOSI
    GPIOC->MODER &= ~(3 << (3 * 2));
    GPIOC->MODER |=  (2 << (3 * 2)); // AF
    GPIOC->AFR[0] &= ~(0xF << (3 * 4));
    GPIOC->AFR[0] |=  (5 << (3 * 4));
    GPIOC->OSPEEDR |= (3 << (3 * 2));

    // PC2 = MISO
    GPIOC->MODER &= ~(3 << (2 * 2));
    GPIOC->MODER |=  (2 << (2 * 2)); // AF
    GPIOC->AFR[0] &= ~(0xF << (2 * 4));
    GPIOC->AFR[0] |=  (5 << (2 * 4));
    GPIOC->OSPEEDR |= (3 << (2 * 2));

    // PB12 = NSS (手动控制/硬件片选)
    GPIOB->MODER &= ~(3 << (12 * 2));
    GPIOB->MODER |=  (1 << (12 * 2)); // 普通输出模式
    GPIOB->OTYPER &= ~(1 << 12);
    GPIOB->OSPEEDR |= (3 << (12 * 2));
    GPIOB->ODR |= (1 << 12); // 默认拉高不选中
}

// SPI2: 主机模式初始化
void SPI2_MasterInit(void) {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    SPI2->CR1 = 0;

    // CPOL=0, CPHA=0（SPI模式0）
    SPI2->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

    // 主机模式
    SPI2->CR1 |= SPI_CR1_MSTR;

    // 软件 NSS 管理
    SPI2->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

    // 波特率 = Fpclk/8（根据你的时钟可以修改）
    SPI2->CR1 |= (0x2 << 3);

    // 8位数据帧（默认）

    // MSB First（默认）

    // 启用 SPI
    SPI2->CR1 |= SPI_CR1_SPE;
}

// 发送一个字节
void SPI2_SendByte(uint8_t data) {
    while (!(SPI2->SR & SPI_SR_TXE));
    SPI2->DR = data;
    while (!(SPI2->SR & SPI_SR_TXE));
    while (SPI2->SR & SPI_SR_BSY);
}

// 接收一个字节（通常用于全双工时发送 dummy 数据获取对方发送的数据）
uint8_t SPI2_ReceiveByte(void) {
    while (!(SPI2->SR & SPI_SR_TXE));
    SPI2->DR = 0xFF; // dummy byte
    while (!(SPI2->SR & SPI_SR_RXNE));
    return SPI2->DR;
}

// 发送并接收一个字节（全双工交换）
uint8_t SPI2_TransferByte(uint8_t data) {
    while (!(SPI2->SR & SPI_SR_TXE));
    SPI2->DR = data;
    while (!(SPI2->SR & SPI_SR_RXNE));
    return SPI2->DR;
}


void SPI2_test(void)
{
	      GPIOB->ODR &= ~(1 << 12);  // 选中从机

        SPI2_SendByte(0x55); // 发送测试字节
        uint8_t recv = SPI2_ReceiveByte(); // 接收响应

        GPIOB->ODR |= (1 << 12);   // 取消片选

        delay_ms(500);
}