//############################################################
// FILE: Tim1_ISR_MCLoop.c
// Created on: 2017年1月15日
// Author: XQ
// summary: Tim1_ISR_MCLoop
//  定时器1中断服务程序，实现电机控制主循环
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
#include "Tim1_ISR_MCLoop.h"
#include "ADC_int.h"          // ADC采样相关
#include "Tim1_PWM.h"         // PWM输出相关
#include "GPIO_int.h"         // GPIO配置相关
#include "ThreeHall.h"        // 霍尔传感器相关
#include "Axis_transform.h"   // 坐标变换相关
#include "Svpwm_dq.h"         // SVPWM控制相关
#include "Task_function.h"    // 任务函数相关
#include "PI_Cale.h"          // PI控制器相关
#include "Timer.h"            // 定时器相关
#include "Usart_RS232.h"      // 串口通信相关
#include "Tim4_Encoder_PWMDAC.h" // 编码器和PWM DAC相关
 
extern  PI_Control   pi_spd ;
extern  PI_Control   pi_id  ;
extern  PI_Control   pi_iq  ;
extern   ADCSamp    ADCSampPare;
extern   uint16_t   PWM_DUTY[3] ;
extern   Hall       Hall_Three;
extern   logic      logicContr;
extern   TaskTime   TaskTimePare;  
extern   CLARKE     ClarkeI;
extern   PARK       ParkI;
extern   IPARK      IparkU;
extern   SVPWM      Svpwmdq;
extern   IQSin_Cos  AngleSin_Cos;
extern   Test       TestPare;
extern   IQAtan     IQAtan_Pare;
 
uint16_t  FilK1=328;
uint16_t  FilK2=696;

/**
 * @brief  定时器1更新中断服务函数
 * @note   实现FOC(磁场定向控制)的核心算法
 *         执行频率：PWM频率（通常为20kHz）
 *         主要功能：
 *         1. 霍尔传感器信号处理和转子位置检测
 *         2. 速度计算（每25个PWM周期执行一次）
 *         3. 三相电流采样和坐标变换
 *         4. 电流环和速度环PI控制
 *         5. SVPWM占空比计算和输出
 */
void TIM1_UP_IRQHandler(void)
{
    /* 1. 中断标志处理 */
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);  // 清除定时器1更新中断标志
    
    /* 2. 霍尔传感器信号处理 */
    ThreeHallanglecale();  // 计算电机转子电角度位置
    
    /* 3. 速度环计算（降频执行） */
    TaskTimePare.PWMZD_count++;  // PWM中断计数器递增
    if(TaskTimePare.PWMZD_count == 25)  // 每25个PWM周期执行一次（降低速度环频率）
    {
        TaskTimePare.PWMZD_count = 0;  // 计数器清零
        
        /* 3.1 速度计算和控制 */
        Hall_Three_Speedcale();  // 根据霍尔信号计算电机速度
        knob_control();          // 处理用户输入的速度指令
        
        /* 3.2 测试数据更新（用于监控和调试） */
        TestPare.id_test = (pi_id.Fbk >> 3);        // d轴电流实际值（右移3位缩小8倍）
        TestPare.iq_test = (pi_iq.Fbk >> 3);        // q轴电流实际值
        TestPare.ud_test = (IparkU.Ds >> 3);        // d轴电压输出值
        TestPare.uq_test = (IparkU.Qs >> 3);        // q轴电压输出值
        
        /* 3.3 系统参数记录 */
        TestPare.fact_BUS_Voil = ADCSampPare.BUS_Voltage;  // 母线电压
        TestPare.fact_BUS_Curr = ADCSampPare.BUS_Curr;     // 母线电流
        TestPare.Speed_fact = (pi_spd.Fbk >> 3);           // 实际速度
        TestPare.Speed_target = (pi_spd.Ref >> 3);         // 目标速度
        
        /* 3.4 速度环PI控制 */
        pi_spd.Fbk = Hall_Three.Speed_RPM;  // 速度反馈值（0-4096RPM）
        PI_Controller((p_PI_Control)&pi_spd);  // 速度PI控制器计算
        /* 速度环输出低通滤波：OutF = K1*OutF + K2*Out */
        pi_spd.OutF = _IQ10mpy(FilK1, pi_spd.OutF) + _IQ10mpy(FilK2, pi_spd.Out);
    }
    
    /* 4. 电流采样和坐标变换 */
    ADC_Sample();  // 采样三相电流和母线电压
    
    /* 4.1 Clarke变换：三相到两相静止坐标系 */
    ClarkeI.As = ADCSampPare.PhaseU_Curr;  // U相电流
    ClarkeI.Bs = ADCSampPare.PhaseV_Curr;  // V相电流
    CLARKE_Cale((p_CLARKE)&ClarkeI);       // 执行Clarke变换
    
    /* 4.2 Park变换：静止坐标系到旋转坐标系 */
    ParkI.Alpha = ClarkeI.Alpha;  // Alpha轴分量
    ParkI.Beta = ClarkeI.Beta;    // Beta轴分量
    
    /* 4.3 计算转子角度的正余弦值 */
    AngleSin_Cos.IQAngle = Hall_Three.ele_angleIQ;  // 电角度
    IQSin_Cos_Cale((p_IQSin_Cos)&AngleSin_Cos);    // 计算正余弦值
    
    /* 4.4 执行Park变换 */
    ParkI.Sine = AngleSin_Cos.IQSin;     // 角度正弦值
    ParkI.Cosine = AngleSin_Cos.IQCos;   // 角度余弦值
    PARK_Cale((p_PARK)&ParkI);           // 执行Park变换
    
    /* 5. 电流环控制 */
    /* 5.1 设置电流环参考值 */
    pi_id.Ref = 0;             // d轴电流参考值为0（最大转矩控制）
    pi_iq.Ref = pi_spd.OutF;   // q轴电流参考值来自速度环输出
    
    /* 5.2 d轴电流PI控制 */
    pi_id.Fbk = ParkI.Ds;  // d轴电流反馈
    PI_Controller((p_PI_Control)&pi_id);  // PI控制器计算
    /* d轴电压输出低通滤波 */
    pi_id.OutF = _IQ10mpy(FilK1, pi_id.OutF) + _IQ10mpy(FilK2, pi_id.Out);
    
    /* 5.3 q轴电流PI控制 */
    pi_iq.Fbk = ParkI.Qs;  // q轴电流反馈
    PI_Controller((p_PI_Control)&pi_iq);  // PI控制器计算
    /* q轴电压输出低通滤波 */
    pi_iq.OutF = _IQ10mpy(FilK1, pi_iq.OutF) + _IQ10mpy(FilK2, pi_iq.Out);
    
    /* 6. 运行模式选择和电压输出处理 */
    if(logicContr.Run_mode == 1)
    {
        /* 模式1：开环控制 */
        IparkU.Ds = 0;                // d轴电压为0
        IparkU.Qs = 10*pi_spd.Ref;    // q轴电压与速度指令成正比
    }
    else if(logicContr.Run_mode == 2)
    {
        /* 模式2：闭环正转控制 */
        IparkU.Ds = pi_id.OutF;     // d轴电压来自d轴电流环
        IparkU.Qs = pi_spd.OutF;    // q轴电压来自速度环
    }
    else if(logicContr.Run_mode == 3)
    {
        /* 模式3：闭环反转控制 */
        IparkU.Ds = -pi_id.OutF;    // d轴电压取反
        IparkU.Qs = -pi_spd.OutF;   // q轴电压取反
    }
    else if(logicContr.Run_mode == 4)
    {
        /* 模式4：双闭环控制（id和iq独立控制）*/
        IparkU.Ds = pi_id.OutF;     // d轴电压来自d轴电流环
        IparkU.Qs = pi_iq.OutF;     // q轴电压来自q轴电流环（最大值为32768）
    }
    
    /* 7. 反Park变换和SVPWM调制 */
    /* 7.1 反Park变换：旋转坐标系到静止坐标系 */
    IparkU.Sine = AngleSin_Cos.IQSin;    // 设置角度正弦值
    IparkU.Cosine = AngleSin_Cos.IQCos;  // 设置角度余弦值
    IPARK_Cale((p_IPARK)&IparkU);        // 执行反Park变换
    
    /* 7.2 SVPWM调制 */
    Svpwmdq.Ualpha = IparkU.Alpha;  // Alpha轴电压分量
    Svpwmdq.Ubeta = IparkU.Beta;    // Beta轴电压分量
    SVPWM_Cale((p_SVPWM)&Svpwmdq);  // 计算SVPWM占空比
    
    /* 7.3 更新PWM输出 */
    Svpwm_Outpwm();  // 更新三相PWM比较值
    
    /* 8. DAC输出用于调试 */
    TIM_SetCompare3(TIM4, (Hall_Three.ele_angleIQ >> 5));  // DAC通道1输出电角度（缩小32倍）
    TIM_SetCompare4(TIM4, PWM_DUTY[0]);                    // DAC通道2输出A相PWM占空比
} 


//===========================================================================
// No more.
//===========================================================================
