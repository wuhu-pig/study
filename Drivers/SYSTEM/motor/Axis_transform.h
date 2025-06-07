/**********************************************************************
 * 文件名: Axis_transform.h
 * 创建日期: 2017年1月5日
 * 作者: XQ
 * 功能概述: 电机控制中的坐标变换头文件，包含Clarke变换和Park变换的数据结构定义
 * 本代码仅用于学习使用，未经允许不得用于其他任何用途
 * 版权所有：硕力电子
 * DSP/STM32开发板设计
 * 淘宝店铺：硕力电子
 * 网址: https://shuolidianzi.taobao.com
 * 修改日期: 2017/1/23
 * 版本：V17.3-1
 * 作者QQ: 616264123
 * 技术交流QQ群：314306105
 **********************************************************************/

#ifndef Axis_transform_H
#define Axis_transform_H

/* 包含所需的头文件 */
#include "./SYSTEM/sys/sys.h"    // 系统配置头文件
#include "IQ_math.h"             // IQ数学库，用于定点数运算

/**
 * @brief  Clarke变换结构体
 * @note   实现三相电流到两相静止坐标系的变换
 */
typedef struct {  
    int32_t  As;      // 输入：A相电流
    int32_t  Bs;      // 输入：B相电流
    int32_t  Cs;      // 输入：C相电流
    int32_t  Alpha;   // 输出：Alpha轴分量
    int32_t  Beta;    // 输出：Beta轴分量
} CLARKE, *p_CLARKE;

/* Clarke变换结构体默认初始值 */
#define CLARKE_DEFAULTS {0,0,0,0,0}

/**
 * @brief  Park变换结构体
 * @note   实现静止坐标系(Alpha-Beta)到旋转坐标系(d-q)的变换
 */
typedef struct {  
    int32_t  Alpha;   // 输入：Alpha轴分量
    int32_t  Beta;    // 输入：Beta轴分量
    int32_t  Angle;   // 输入：转子角度（标幺值）
    int32_t  Ds;      // 输出：d轴分量
    int32_t  Qs;      // 输出：q轴分量
    int32_t  Sine;    // 中间变量：角度正弦值
    int32_t  Cosine;  // 中间变量：角度余弦值
} PARK, *p_PARK;

/* Park变换结构体默认初始值 */
#define PARK_DEFAULTS {0,0,0,0,0,0,0}

/**
 * @brief  反Park变换结构体
 * @note   实现旋转坐标系(d-q)到静止坐标系(Alpha-Beta)的变换
 */
typedef struct {  
    int32_t  Alpha;   // 输出：Alpha轴分量
    int32_t  Beta;    // 输出：Beta轴分量
    int32_t  Angle;   // 输入：转子角度（标幺值）
    int32_t  Ds;      // 输入：d轴分量
    int32_t  Qs;      // 输入：q轴分量
    int32_t  Sine;    // 输入：角度正弦值
    int32_t  Cosine;  // 输入：角度余弦值
} IPARK, *p_IPARK;

/* 反Park变换结构体默认初始值 */
#define IPARK_DEFAULTS {0,0,0,0,0,0,0}

/**
 * 函数声明
 */
/* Clarke变换计算函数 */
void CLARKE_Cale(p_CLARKE pV);
/* Park变换计算函数 */
void PARK_Cale(p_PARK pV);
/* 反Park变换计算函数 */
void IPARK_Cale(p_IPARK pV);

#endif /* Axis_transform_H */

//===========================================================================
// 文件结束
//===========================================================================
