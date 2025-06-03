#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h" 
#include "as5600.h"
#include "atim.h"
#include "gtim.h"
#include "adc.h"
#include "dma.h"
#include "QDTFT_demo.h"
#include "lcd_drive.h"
#include "GUI.h"
#include "math.h"
#include "motor.h"
extern uint8_t g_adc_dma_sta;                   /* DMA传输状态标志, 0,未完成; 1, 已完成 */
int main(void)
{
//    uint16_t times = 0;
//		uint8_t dir = 1;
		float t = 0;
		status_t retval;
    sys_stm32_clock_init(336, 8, 2, 7);     /* 设置时钟,168Mhz */
    delay_init(168);                        /* 延时初始化 */
    usart_init(84, 115200);                 /* 串口初始化为115200 */
    led_init(); 														//LED初始化                         	
		iic_init();															//I2C初始化
		adc_nch_dma_init((uint32_t)&g_adc_dma_buf); /* 初始化ADC DMA采集 */
		adc_dma_enable(ADC_DMA_BUF_SIZE);  	 /* 启动ADC DMA采集 5v对应4096 */
		Lcd_Init();
		motor_init();
    atim_timx_cplm_pwm_init(1000 - 1, 168 - 1); /* 168/4=42Mhz的计数频率 1Khz的周期. */  //1ms
		gtim_timx_int_init(100-1, 8400 - 1); /* 84 000 000 / 84 00 = 10 000 10Khz的计数频率，计数5K次为500ms */	//1ms
		while (1)
    {
			Programe_Run();												//AS5600编码器
			RunSystimer();
			
			if(TaskTimePare.Tim1s_flag == 1)
			{
					LED0_TOGGLE();
			}
			if(TaskTimePare.Tim10ms_flag == 1)
			{
				
				printf("d:%f,%f,%d,%d\n",shaft_angle,degress,ADCSampPare.PhaseW_Curr,ADCSampPare.EA_Curr);
			//	printf("d: %f\n",shaft_angle);
			}
			CLEAR_flag();
    }
}

