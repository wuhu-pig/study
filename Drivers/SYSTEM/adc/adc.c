#include "adc.h"
#include "led.h"
#include "dma.h"
#include "delay.h"

uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE]; 
uint8_t g_adc_dma_sta = 0;  /* DMA传输状态标志, 0,未完成; 1, 已完成 */
uint16_t adc_buffer[ADC_CHANNEL_NUM]; 

void adc_init(void)
{
    // 1. 时钟使能
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    // GPIOA时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;     // DMA2时钟
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;     // ADC1时钟

    // 2. 配置PA0~PA2为模拟输入
    GPIOA->MODER |= (3 << (0 * 2)) | (3 << (1 * 2)) | (3 << (2 * 2));

    // 3. 配置ADC时钟
    ADC->CCR |= (1 << 16); // PCLK2 / 4 = 84MHz / 4 = 21MHz

    // 4. 配置ADC工作模式
    ADC1->CR1 = ADC_CR1_SCAN;                            // 启用扫描模式
    ADC1->CR2 = ADC_CR2_DMA | ADC_CR2_DDS;               // DMA模式 + DMA自动请求
    ADC1->CR2 &= ~ADC_CR2_EOCS;                          // 只在序列完成后触发EOC
    ADC1->CR2 |= (1 << 0);                               // 开启ADC
    ADC1->CR2 &= ~(1 << 1);                              // 关闭连续转换（使用外部触发）

    // 5. 配置外部触发：TIM8_TRGO（EXTSEL = 0b1110）
    ADC1->CR2 &= ~(0xF << 24);                           // 清除EXTSEL
    ADC1->CR2 |= (0xE << 24);                            // EXTSEL = 0b1110
    ADC1->CR2 |= (1 << 28);                              // EXTEN = 0b01 => 上升沿触发

    // 6. 配置通道序列
    ADC1->SQR1 &= ~(0xF << 20);                          // 清除L位
    ADC1->SQR1 |= (4 << 20);                             // L = 2 => 3个通道
    ADC1->SQR3 = (0 << 0) | (1 << 5) | (2 << 10) | (3 << 10) | (4 << 10) | (5 << 10);        // 顺序 IN0 → IN1 → IN2 → IN3 → IN4 → IN5
}

void dma_init(void)
{
    DMA2_Stream0->CR = 0;
    while(DMA2_Stream0->CR & DMA_SxCR_EN);              // 等待DMA关闭
    DMA2->LIFCR |= 0x3D << 0;                           // 清除所有通道0中断标志

    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;            // ADC数据寄存器
    DMA2_Stream0->M0AR = (uint32_t)adc_buffer;          // 内存地址
    DMA2_Stream0->NDTR = ADC_CHANNEL_NUM;               // 数据数量 = 3
    DMA2_Stream0->CR |= (0 << 6);                       // 外设到内存
    DMA2_Stream0->CR |= (1 << 8);                       // 外设地址不变
    DMA2_Stream0->CR |= (1 << 10);                      // 内存地址递增
    DMA2_Stream0->CR |= (1 << 11);                      // 外设数据宽度：16位
    DMA2_Stream0->CR |= (1 << 13);                      // 内存数据宽度：16位
    DMA2_Stream0->CR |= (0 << 16);                      // 单次模式
    DMA2_Stream0->CR |= (0 << 21);                      // 通道选择：通道0（ADC1）
    DMA2_Stream0->CR |= DMA_SxCR_TCIE;                  // 开启传输完成中断

    NVIC_SetPriority(DMA2_Stream0_IRQn, 1);
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    DMA2_Stream0->CR |= DMA_SxCR_EN;                    // 启动DMA
}


void DMA2_Stream0_IRQHandler(void)
{
    if(DMA2->LISR & DMA_LISR_TCIF0)
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;  // 清除中断标志

        // 处理ADC结果
			LED0_TOGGLE();

        // 用户处理逻辑...
    }
}







