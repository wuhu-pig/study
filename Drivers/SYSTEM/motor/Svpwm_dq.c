//############################################################
// FILE:  Svpwm_dq.c
// Created on: 2017年1月18日
// Author: XQ
// summary: Svpwm_dq
//本代码仅用于学习使用，未经允许不得用于其他任何用途
//版权所有：硕力电子
//DSP/STM32开发板设计
//淘宝店铺：硕力电子
//网址: https://shuolidianzi.taobao.com
//修改日期: 2017/1/23
//版本：V17.3-1         
//Author-QQ: 616264123
//技术交流QQ群：314306105
//############################################################

/* 包含头文件 */
#include "Svpwm_dq.h"

/**
 * @brief  SVPWM计算函数
 * @param  pV: SVPWM结构体指针
 * @note   实现空间矢量PWM的扇区判断和占空比计算
 *         1. 首先计算三个中间变量用于扇区判断
 *         2. 根据中间变量的符号确定电压矢量所在扇区(1-6)
 *         3. 根据扇区计算三相PWM的占空比
 */
void SVPWM_Cale(p_SVPWM pV)
{
    /* 计算用于扇区判断的中间变量 */
    pV->tmp1 = pV->Ubeta;                                    // X = Ubeta
    pV->tmp2 = _IQdiv2(pV->Ubeta) + _IQmpy(28377, pV->Ualpha); // Y = Ubeta/2 + √3/2*Ualpha
    pV->tmp3 = pV->tmp2 - pV->tmp1;                         // Z = Y - X

    /* 扇区判断 
     * 通过判断X、Y、Z的符号确定电压矢量所在扇区
     * 28377为√3/2的Q15定点数表示(0.866)
     */
    pV->VecSector = 3;
    pV->VecSector = (pV->tmp2 > 0) ? (pV->VecSector - 1) : pV->VecSector;  // 判断Y的符号
    pV->VecSector = (pV->tmp3 > 0) ? (pV->VecSector - 1) : pV->VecSector;  // 判断Z的符号
    pV->VecSector = (pV->tmp1 < 0) ? (7 - pV->VecSector) : pV->VecSector;  // 判断X的符号

    /* 根据扇区计算三相PWM占空比 */
    if (pV->VecSector == 1 || pV->VecSector == 4)
    {  
        /* 扇区1和4的PWM占空比计算 */
        pV->Ta = pV->tmp2;           // Ta = Y
        pV->Tb = pV->tmp1 - pV->tmp3;// Tb = X - Z
        pV->Tc = -pV->tmp2;          // Tc = -Y
    }
    else if (pV->VecSector == 2 || pV->VecSector == 5)
    {  
        /* 扇区2和5的PWM占空比计算 */
        pV->Ta = pV->tmp3 + pV->tmp2;// Ta = Z + Y
        pV->Tb = pV->tmp1;           // Tb = X
        pV->Tc = -pV->tmp1;          // Tc = -X
    }
    else
    {   
        /* 扇区3和6的PWM占空比计算 */
        pV->Ta = pV->tmp3;           // Ta = Z
        pV->Tb = -pV->tmp3;          // Tb = -Z
        pV->Tc = -(pV->tmp1 + pV->tmp2); // Tc = -(X + Y)
    }
}

//===========================================================================
// No more.
//===========================================================================
