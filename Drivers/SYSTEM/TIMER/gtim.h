/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-12-31
 * @brief       通用定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211231
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"
typedef struct {	
	      uint8_t   PWMZD_count;
        uint8_t   IntClock_1ms;	 
        uint8_t   Tim1ms_flag;	
				uint8_t   Tim10ms_count;	
        uint8_t   Tim10ms_flag;
        uint8_t   Tim100ms_count;
        uint8_t   Tim100ms_flag;
        uint8_t   Tim500ms_count;
        uint8_t   Tim500ms_flag; 
        uint8_t   Tim1s_count;
        uint8_t   Tim1s_flag ;
        uint8_t   Tim10s_count;
        uint8_t   Tim10s_flag ;
        uint8_t   Tim1min_count;
        uint8_t   Tim1min_flag ;
	   }TaskTime;

#define  TaskTime_DEFAULTS  {0,0,0,0,0,0,0,0,0,0,0,0,0}
/******************************************************************************************/
/* 通用定时器 定义 */

/* TIMX 中断定义 
 * 默认是针对TIM2~TIM5, TIM9~TIM14.
 * 注意: 通过修改这4个宏定义,可以支持TIM1~TIM14任意一个定时器.
 */
 
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ RCC->APB1ENR |= 1 << 1; }while(0)  /* TIM3 时钟使能 */

/******************************************************************************************/
extern TaskTime TaskTimePare;
extern void gtim_timx_int_init(uint16_t arr, uint16_t psc);    /* 通用定时器 定时中断初始化函数 */
extern void RunSystimer(void);
extern void CLEAR_flag(void);
#endif

















