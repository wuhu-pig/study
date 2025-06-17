#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"

#define ADC_DMA_BUF_SIZE        50 * 6          /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */

typedef struct {
	     int32_t   BUS_Curr ;     // DC Bus  Current
	     int32_t   PhaseU_Curr;   // Phase A Current
	     int32_t   PhaseV_Curr;   // Phase B Current
			 int32_t   PhaseW_Curr;   // Phase B Current
	     int32_t   BUS_Voltage ;  //DC Bus  Voltage	     
	     int32_t   RP_speed_Voltage ;     //  RP1_Voltage
	     int32_t   OffsetBUS_Curr ;     // DC Bus  Current
	     int32_t   OffsetPhaseU_Curr;   // Phase A Current
	     int32_t   OffsetPhaseV_Curr;   // Phase B Current
			 int32_t   OffsetPhaseW_Curr;   // Phase B Current
		   int32_t   EA_Curr;   // Phase A Current
	     int32_t   EB_Curr;   // Phase B Current
			 int32_t   EC_Curr;   // Phase B Current
		   int32_t   OffsetEA_Curr;   // Phase A Current
	     int32_t   OffsetEB_Curr;   // Phase B Current
			 int32_t   OffsetEC_Curr;   // Phase B Current
	     int32_t   Coeff_filterK1;   // Phase A Current
		   int32_t   Coeff_filterK2;   // Phase B Current 
			 int32_t   Coeff_filterK3;   // Phase B Current 
	}ADCSamp;

#define  ADCSamp_DEFAULTS  {0,0,0,0,0,0,0,0,268,756}

/******************************************************************************************/
/* ADC及引脚 定义 */

#define ADC_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_ADCX_CHY_GPIO_PIN               SYS_GPIO_PIN5 
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()      do{ RCC->AHB1ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CHY                        5                                       /* 通道Y,  0 <= Y <= 19 */ 
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ RCC->APB2ENR |= 1 << 8; }while(0)   /* ADC1 时钟使能 */
#define ADC_CHANNEL_NUM  5  // 比如采集3路ADC通道


/* ADC单通道/多通道 DMA采集 DMA数据流相关 定义 
 * 注意: 这里我们的通道还是使用上面的定义.
 */
#define ADC_ADCX_DMASx                      DMA2_Stream4
#define ADC_ADCX_DMASx_Channel              0                                       /* 通道0 */
#define ADC_ADCX_DMASx_IRQn                 DMA2_Stream4_IRQn
#define ADC_ADCX_DMASx_IRQHandler           DMA2_Stream4_IRQHandler

#define ADC_ADCX_DMASx_IS_TC()              ( DMA2->HISR & (1 << 5) )   /* 判断 DMA2_Stream4 传输完成标志, 这是一个假函数形式,
                                                                         * 不能当函数使用, 只能用在if等语句里面 
                                                                         */
#define ADC_ADCX_DMASx_CLR_TC()             do{ DMA2->HIFCR |= 1 << 5; }while(0)    /* 清除 DMA2_Stream4 传输完成标志 */

/******************************************************************************************/
extern  uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];       /* ADC DMA BUF */
extern uint8_t g_adc_dma_sta;     
extern uint16_t adc_buffer[ADC_CHANNEL_NUM];  // DMA搬运目标地址
extern void adc_init(void);
extern void dma_init(void);
#endif 















