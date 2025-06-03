//############################################################
// FILE:  Axis_transform.c
// Created on: 2017年1月11日
// Author: XQ
// summary:Axis_transform
//本代码仅用于学习使用，未经允许不得用于其他任何用途
//版权所有：硕力电子
//DSP/STM32开发板设计
//淘宝店铺：硕力电子
//网址: https://shuolidianzi.taobao.com
//修改日期: 2017/1/24
//版本：V17.3-1
//Author-QQ: 616264123
//技术交流QQ群：314306105
//############################################################

#include "Axis_transform.h"
    
/**
 * @brief  Clarke变换计算函数
 * @param  pV: Clarke变换结构体指针
 * @note   三相到两相静止坐标系变换
 *         Alpha = Iu
 *         Beta = (√3/3*2^15*Iu + 2*√3/3*2^15*Iw)/(2^15)
 *         其中18918为0.57735026918963的Q15定点数表示
 */
void  CLARKE_Cale(p_CLARKE  pV)
{
	/* Alpha轴分量等于A相电流 */
	pV->Alpha = pV->As;
	/* Beta轴分量计算 */
	pV->Beta = _IQmpy((pV->As +_IQmpy2(pV->Bs)),18918);  //  _IQ(0.57735026918963)
}

/**
 * @brief  Park变换计算函数
 * @param  pV: Park变换结构体指针
 * @note   静止坐标系到旋转坐标系变换
 *         Id = Ialpha*cos+Ibeta*sin
 *         Iq = Ibeta*cos-Ialpha*sin
 */
void  PARK_Cale(p_PARK pV)
{
	pV->Ds = _IQmpy(pV->Alpha,pV->Cosine) + _IQmpy(pV->Beta,pV->Sine);
  pV->Qs = _IQmpy(pV->Beta,pV->Cosine) - _IQmpy(pV->Alpha,pV->Sine);
}

/**
 * @brief  反Park变换计算函数
 * @param  pV: 反Park变换结构体指针
 * @note   旋转坐标系到静止坐标系变换
 *         Ialpha = Id*cos - Iq*sin
 *         Ibeta = Iq*cos + Id*sin
 */
void  IPARK_Cale(p_IPARK pV)    
{
	pV->Alpha = _IQmpy(pV->Ds,pV->Cosine) - _IQmpy( pV->Qs,pV->Sine);
	pV->Beta  = _IQmpy(pV->Qs,pV->Cosine) + _IQmpy(pV->Ds,pV->Sine);
}

//===========================================================================
// No more.
//===========================================================================

