//############################################################
// FILE: Timer.h
// Created on: 2017年1月18日
// Author: XQ
// summary: Timer_ 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//DSP/STM32电机控制开发板
//硕历电子
//网址: https://shuolidianzi.taobao.com
//修改日期:2017/1/23
//版本：V17.3-1
//Author-QQ: 616264123
//电机控制QQ群：314306105
//############################################################
   
#ifndef _TIMER_H
#define _TIMER_H 

#include "stm32f10x.h"

typedef struct {	
	      uint8_t   PWMZD_count;	
        uint8_t   IntClock_10ms;	 
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
   
void SysTickConfig(void);
void RunSystimer(void);
void CLEAR_flag(void);

#endif /* __TIMER_H */

//===========================================================================
// No more.
//===========================================================================
