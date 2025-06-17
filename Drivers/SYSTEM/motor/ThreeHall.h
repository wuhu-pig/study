//############################################################
// FILE:  ThreeHall.h
// Created on: 2017年1月5日
// Author: XQ
// summary: Header file  and definition
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

#ifndef ThreeHall_H
#define ThreeHall_H
#include "stm32f10x.h"
#include "IQ_math.h"

typedef struct {
	    uint8_t       HallUVW[3];
	    uint8_t       Hall_State;
	    uint8_t       OldHall_State;
	    uint8_t       HallLX_State;
	    uint8_t       Hall_num[8];
	    int32_t       Hall_angle[8];
			int32_t       step_angle_error;
	    int32_t       step_angle ;
			int32_t       step_angleFitter ;
	    uint16_t      Speed_count;
	    uint16_t      Speed_countFitter;
      uint16_t      Speed_count_old;
			uint32_t      speed_coeff;
	    uint8_t       Poles;
	    uint8_t       Move_State;
      int32_t       initial_angle;
      int32_t       angleIQ;
      uint16_t      Speed_RPM;      
      int32_t       Speed_ele_angleIQ;
      int32_t       Speed_ele_angleIQFitter;
      int32_t       old_ele_angleIQ;
      int32_t       ele_angleIQ;
	   } Hall;

#define  Hall_DEFAULTS {0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0}

void ThreeHallPara_init(void);
void ThreeHallanglecale(void);
void Hall_Three_Speedcale(void);
#endif /* ThreeHall_H_*/
//===========================================================================
// End of file.
//===========================================================================
