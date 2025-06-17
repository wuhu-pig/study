//############################################################
// FILE:   Usart_RS232.h
// Created on: 2017年1月21日
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
 
#ifndef Usart_RS232_H
#define Usart_RS232_H
#include "IQ_math.h"
#include "stm32f10x.h"

typedef struct {		     
        uint8_t   Uart_txdr[8];
        uint8_t   Uart_rxdr[8];
	      uint16_t  fact_BUS_Curr;
	      uint16_t  fact_BUS_Voil;
		    uint16_t  iq_test;
	      uint16_t  id_test;
	      uint16_t  uq_test;
	      uint16_t  ud_test;
	      int16_t   IqRef_test;
	      uint16_t  Speed_target;
	      uint16_t  Speed_fact;
        uint8_t   Rece_flag;
	      uint8_t   Rxdr_byte;
        uint8_t   chaoshi_jishu;
        uint8_t   chaoshi_pand;
        uint8_t   chaoshi_pandold;
        uint8_t   chaoshi_shu;
	   }Test;

#define  Test_DEFAULTS  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
 
void Uart3_RS232TX_sen(void);
void Usart3_RS232_init(void);
void Uart2_Sendlen(uint8_t *data,u8 len);
void USART3_IRQHandler(void);  
void  ReceiveData_chuli(void);
#endif  // end of SCI_RS232.h definition

//===========================================================================
// End of file.
//===========================================================================
