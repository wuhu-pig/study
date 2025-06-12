#ifndef __ATIM_H
#define __ATIM_H

#include "sys.h"

/******************************************************************************************/
/*
 * 注意: 通过修改这些宏定义,可以支持TIM1/TIM8定时器, 任意一个IO口输出互补PWM(前提是必须有互补输出功能)
 */

/* A_Phase输出通道引脚 */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT_A            GPIOC
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN_A            	SYS_GPIO_PIN6
#define ATIM_TIMX_CPLM_CHY_GPIO_AF_A              2                                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE_A()    do{ RCC->AHB1ENR |= 1 << 2; }while(0)       /* PE口时钟使能 */

/* A_Phase互补输出通道引脚 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT_A           GPIOA
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN_A            SYS_GPIO_PIN7
#define ATIM_TIMX_CPLM_CHYN_GPIO_AF_A             4                                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE_A()   do{ RCC->AHB1ENR |= 1 << 0; }while(0)       /* PE口时钟使能 */

/* B_Phase输出通道引脚 */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT_B            GPIOC
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN_B            	SYS_GPIO_PIN7
#define ATIM_TIMX_CPLM_CHY_GPIO_AF_B              2                                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE_B()    do{ RCC->AHB1ENR |= 1 << 2; }while(0)       /* PE口时钟使能 */

/* BPhase互补输出通道引脚 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT_B           GPIOB
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN_B            SYS_GPIO_PIN14
#define ATIM_TIMX_CPLM_CHYN_GPIO_AF_B             3                                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE_B()   do{ RCC->AHB1ENR |= 1 << 4; }while(0)       /* PE口时钟使能 */

/* C_Phase输出通道引脚 */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT_C            GPIOC
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN_C            	SYS_GPIO_PIN8
#define ATIM_TIMX_CPLM_CHY_GPIO_AF_C              2                                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE_C()    do{ RCC->AHB1ENR |= 1 << 2; }while(0)       /* PE口时钟使能 */

/* C_Phase互补输出通道引脚 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT_C           GPIOB
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN_C            SYS_GPIO_PIN15
#define ATIM_TIMX_CPLM_CHYN_GPIO_AF_C             3                                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE_C()   do{ RCC->AHB1ENR |= 1 << 4; }while(0)       /* PE口时钟使能 */

/* 刹车输入引脚 */
#define ATIM_TIMX_CPLM_BKIN_GPIO_PORT           	GPIOA
#define ATIM_TIMX_CPLM_BKIN_GPIO_PIN            	SYS_GPIO_PIN6
#define ATIM_TIMX_CPLM_BKIN_GPIO_AF             	4                                           /* AF功能选择 */
#define ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE()   	do{ RCC->AHB1ENR |= 1 << 0; }while(0)       /* PE口时钟使能 */

/* 互补输出使用的定时器 */
#define ATIM_TIMX_CPLM                          		TIM8
#define ATIM_TIMX_CPLM_CHY_A                      	1
#define ATIM_TIMX_CPLM_CHY_CCRY_A                 	ATIM_TIMX_CPLM->CCR1
#define ATIM_TIMX_CPLM_CHY_B                      	2
#define ATIM_TIMX_CPLM_CHY_CCRY_B                 	ATIM_TIMX_CPLM->CCR2
#define ATIM_TIMX_CPLM_CHY_C                      	3
#define ATIM_TIMX_CPLM_CHY_CCRY_C                 	ATIM_TIMX_CPLM->CCR3
#define ATIM_TIMX_CPLM_CLK_ENABLE()             	do{ RCC->APB2ENR |= 1 << 1; }while(0)       /* TIM2 时钟使能 */
/******************************************************************************************/

void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);   /* 高级定时器 互补输出 初始化函数 */
void atim_timx_cplm_pwm_set(uint16_t ccr, uint8_t channel);
#endif













