#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h" 
#include "as5600.h"
#include "atim.h"
#include "gtim.h"
#include "adc.h"
#include "QDTFT_demo.h"
#include "lcd_drive.h"
#include "GUI.h"
#include "math.h"
#include "motor.h"
#include "i2c.h"
#include "spi.h"
#include "foc.h"

                  /* DMA传输状态标志, 0,未完成; 1, 已完成 */
uint16_t angle;
int main(void)
{
		float t = 0;
		status_t retval;
    sys_stm32_clock_init(336, 8, 2, 7);     /* 设置时钟,168Mhz */
    delay_init(168);                        /* 延时初始化 */
    usart_init(84, 115200);                 /* 串口初始化为115200 */
    led_init(); 														//LED初始化                         	
		//iic_init();															//I2C初始化
		//Lcd_Init();
		//motor_init();
		SPI2_GPIO_MasterInit();
    SPI2_MasterInit();
		AS5600_Init();
		adc_init();
    dma_init();
    atim_timx_cplm_pwm_init(1000 - 1, 168 - 1); /* 168/4=42Mhz的计数频率 1Khz的周期. */  //1ms
		gtim_timx_int_init(100-1, 8400 - 1); /* 84 000 000 / 84 00 = 10 000 10Khz的计数频率，计数5K次为500ms */	//1ms
		foc_init();
		while (1)
    {
			RunSystimer();
			if(TaskTimePare.Tim10ms_flag == 1)
			{
					printf("d:%d,%d,%d\n",my_motor.control.CCR.ccra,my_motor.control.CCR.ccrb,my_motor.control.CCR.ccrc);
			}
			CLEAR_flag();
    }
}

