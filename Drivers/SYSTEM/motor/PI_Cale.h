/**********************************************************************
 * 文件名: PI_Cale.h
 * 创建日期: 2017年1月11日
 * 作者: XQ
 * 功能概述: PI控制器头文件，包含数据结构定义和函数声明
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

#ifndef PI_Cale_H
#define PI_Cale_H

/* 包含所需的头文件 */
#include "IQ_math.h"        // IQ数学库，用于定点数运算
#include "./SYSTEM/sys/sys.h"  // 系统配置头文件

/**
 * @brief  PI控制器数据结构
 * @note   包含PI控制器所需的所有参数和中间计算数据
 */
typedef struct {
    int32_t  Ref;          // 输入：参考值（目标值）
    int32_t  Fbk;          // 输入：反馈值（实际值）
    int32_t  Out;          // 输出：控制器输出值
    int32_t  OutF;         // 输出：滤波后的控制器输出值
    int32_t  Kp;           // 参数：比例增益
    int32_t  Ki;           // 参数：积分增益
    int32_t  Umax;         // 参数：输出上限值
    int32_t  Umin;         // 参数：输出下限值
    int32_t  up;           // 数据：比例项计算值
    int32_t  ui;           // 数据：积分项计算值
    int32_t  v1;           // 数据：限幅前的控制器输出
    int32_t  i1;           // 数据：积分项历史值 ui(k-1)
} PI_Control, *p_PI_Control;

/* PI控制器结构体默认初始值 */
#define PI_Control_DEFAULTS {0,0,0,0,0,0,32000,0,0,0,0,0}

/* 全局PI控制器实例声明 */
extern PI_Control pi_spd;   // 速度环PI控制器
extern PI_Control pi_id;    // d轴电流PI控制器
extern PI_Control pi_iq;    // q轴电流PI控制器

/**
 * 函数声明
 */
/* PI控制器计算函数 */
void PI_Controller(p_PI_Control pV);
/* PI控制器参数初始化函数 */
void PI_Pare_init(void);

#endif /* PI_Cale_H */
