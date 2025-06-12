#include "atim.h"
#include "led.h"
#include "adc.h"
#include "foc.h"
/**
 * @brief       高级定时器TIMX 互补输出 初始化函数（使用PWM模式1）
 * @note
 *              配置高级定时器TIMX 互补输出, 一路OCy 一路OCyN, 并且可以设置死区时间
 *
 *              高级定时器的时钟来自APB2,当PPRE2≥2分频的时候
 *              高级定时器的时钟为APB2时钟的2倍, 而APB2为84M, 所以定时器时钟 = 168Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
    /*A_Phase*/
    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE_A();   /* 通道X对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE_A();  /* 通道X互补通道对应IO口时钟使能 */
		/*B_Phase*/
	  ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE_B();   /* 通道X对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE_B();  /* 通道X互补通道对应IO口时钟使能 */
		/*C_Phase*/
	   ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE_C();   /* 通道X对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE_C();  /* 通道X互补通道对应IO口时钟使能 */
	
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE();  /* 通道X刹车输入对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CLK_ENABLE();            /* 使能定时器时钟 */

    sys_gpio_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT_A, ATIM_TIMX_CPLM_CHY_GPIO_PIN_A,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHX 引脚模式设置 */
    sys_gpio_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT_A, ATIM_TIMX_CPLM_CHYN_GPIO_PIN_A,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHXN 引脚模式设置 */
	
    sys_gpio_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT_B, ATIM_TIMX_CPLM_CHY_GPIO_PIN_B,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHX 引脚模式设置 */
    sys_gpio_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT_B, ATIM_TIMX_CPLM_CHYN_GPIO_PIN_B,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHXN 引脚模式设置 */ 
								 
		sys_gpio_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT_C, ATIM_TIMX_CPLM_CHY_GPIO_PIN_C,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHX 引脚模式设置 */
    sys_gpio_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT_C, ATIM_TIMX_CPLM_CHYN_GPIO_PIN_C,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_CHXN 引脚模式设置 */
								 
    sys_gpio_set(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, ATIM_TIMX_CPLM_BKIN_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* TIMX_BKIN 引脚模式设置 */

    sys_gpio_af_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT_A,  ATIM_TIMX_CPLM_CHY_GPIO_PIN_A,  ATIM_TIMX_CPLM_CHY_GPIO_AF_A);   /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT_A, ATIM_TIMX_CPLM_CHYN_GPIO_PIN_A, ATIM_TIMX_CPLM_CHYN_GPIO_AF_A);  /* IO口复用功能选择 必须设置对!! */
		
		sys_gpio_af_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT_B,  ATIM_TIMX_CPLM_CHY_GPIO_PIN_B,  ATIM_TIMX_CPLM_CHY_GPIO_AF_B);   /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT_B, ATIM_TIMX_CPLM_CHYN_GPIO_PIN_B, ATIM_TIMX_CPLM_CHYN_GPIO_AF_B);  /* IO口复用功能选择 必须设置对!! */
		
		sys_gpio_af_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT_C,  ATIM_TIMX_CPLM_CHY_GPIO_PIN_C,  ATIM_TIMX_CPLM_CHY_GPIO_AF_C);   /* IO口复用功能选择 必须设置对!! */
    sys_gpio_af_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT_C, ATIM_TIMX_CPLM_CHYN_GPIO_PIN_C, ATIM_TIMX_CPLM_CHYN_GPIO_AF_C);  /* IO口复用功能选择 必须设置对!! */
		
    sys_gpio_af_set(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, ATIM_TIMX_CPLM_BKIN_GPIO_PIN, ATIM_TIMX_CPLM_BKIN_GPIO_AF);  /* IO口复用功能选择 必须设置对!! */

    ATIM_TIMX_CPLM->ARR = arr;          /* 设定计数器自动重装值 */
    ATIM_TIMX_CPLM->PSC = psc;          /* 设置预分频器 */
		
		
		for(int chy =1;chy<4;chy++ )
		{
				if (chy <= 2)
				{
					ATIM_TIMX_CPLM->CCMR1 |= 6 << (4 + 8 * (chy - 1));  /* CH1/2 PWM模式1 */
					ATIM_TIMX_CPLM->CCMR1 |= 1 << (3 + 8 * (chy - 1));  /* CH1/2 预装载使能 */
				}
				else if (chy <= 4)
				{
					ATIM_TIMX_CPLM->CCMR2 |= 6 << (4 + 8 * (chy - 3));  /* CH3/4 PWM模式1 */
					ATIM_TIMX_CPLM->CCMR2 |= 1 << (3 + 8 * (chy - 3));  /* CH3/4 预装载使能 */
				}
					ATIM_TIMX_CPLM->CCER |= 1 << (4 * (chy - 1));       /* OCy 输出使能 */
					ATIM_TIMX_CPLM->CCER |= 1 << (1 + 4 * (chy - 1));   /* OCy 低电平有效 */
					ATIM_TIMX_CPLM->CCER |= 1 << (2 + 4 * (chy - 1));   /* OCyN 输出使能 */
					ATIM_TIMX_CPLM->CCER |= 1 << (3 + 4 * (chy - 1));   /* OCyN 低电平有效 */
		
		}

    // 关键修改部分
    ATIM_TIMX_CPLM->CR1 &= ~(0x03 << 5);        // 清除CMS位
    ATIM_TIMX_CPLM->CR1 |=  (0x01 << 5);        // CMS=01 (中心对齐模式1)
    ATIM_TIMX_CPLM->CR1 |=  (0x01 << 7);        // ARPE使能
    ATIM_TIMX_CPLM->CR1 |=  (0x02 << 8);        // CKD=10 (时钟分频)

    // 配置TRGO在中心点触发(计数器为0时)
    ATIM_TIMX_CPLM->CR2 &= ~(0x07 << 4);        // 清除MMS位
    ATIM_TIMX_CPLM->CR2 |=  (0x02 << 4);        // MMS=010 (更新事件作为TRGO)

    // 刹车配置
    ATIM_TIMX_CPLM->BDTR |= 0 << 16;            // BKF[3:0]=0 (不滤波)
    ATIM_TIMX_CPLM->BDTR |= 1 << 14;            // AOE=1 (自动恢复输出)
    ATIM_TIMX_CPLM->BDTR |= 0 << 13;            // BKP=0 (低电平有效)
    ATIM_TIMX_CPLM->BDTR |= 1 << 12;            // BKE=1 (刹车使能)

    // 中断配置
    ATIM_TIMX_CPLM->DIER |= 1 << 0;             // 使能更新中断(UIE)
    ATIM_TIMX_CPLM->SR  &= ~(1 << 0);           // 清除更新标志
    NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 1);    // 设置中断优先级
    NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);         // 使能中断通道

    ATIM_TIMX_CPLM->CR1 |= 1 << 0;              // 使能定时器
}

/**
 * @brief       定时器TIMX 设置输出比较值 & 死区时间
 * @param       ccr: 输出比较值
 * @param       dtg: 死区时间
 *   @arg       dtg[7:5]=0xx时, 死区时间 = dtg[7:0] * tDTS
 *   @arg       dtg[7:5]=10x时, 死区时间 = (64 + dtg[6:0]) * 2  * tDTS
 *   @arg       dtg[7:5]=110时, 死区时间 = (32 + dtg[5:0]) * 8  * tDTS
 *   @arg       dtg[7:5]=111时, 死区时间 = (32 + dtg[5:0]) * 16 * tDTS
 *   @note      tDTS = 1 / (Ft /  CKD[1:0]) = 1 / 42M = 23.8ns
 * @retval      无
 */
void atim_timx_cplm_pwm_set(uint16_t ccr, uint8_t channel)
{
    /* 清零 DTG[7:0] */
    ATIM_TIMX_CPLM->BDTR &= ~(0XFF << 0);
    /* 设置 DTG[7:0] */
    ATIM_TIMX_CPLM->BDTR |= 100 << 0;
    /* 使能主输出 */
    ATIM_TIMX_CPLM->BDTR |= 1 << 15;
    /* 设置比较寄存器 */
    switch(channel) {
        case 1:
            ATIM_TIMX_CPLM_CHY_CCRY_A = ccr;
            break;
        case 2:
            ATIM_TIMX_CPLM_CHY_CCRY_B = ccr;
            break;
        case 3:
            ATIM_TIMX_CPLM_CHY_CCRY_C = ccr;
            break;
        default:
            break;
    }
}

// 在stm32f4xx_it.c中添加
void TIM8_UP_TIM13_IRQHandler(void)
{
    if (ATIM_TIMX_CPLM->SR & TIM_SR_UIF) {
        // 清除中断标志
        ATIM_TIMX_CPLM->SR &= ~TIM_SR_UIF;
				LED0_TOGGLE();
				foc_main();
       // adc_dma_enable(ADC_DMA_BUF_SIZE);
        // 这里添加您的处理代码
        // 例如：HAL_GPIO_TogglePin(GPIOx, GPIO_PIN_x); // 测试用
    }
}
