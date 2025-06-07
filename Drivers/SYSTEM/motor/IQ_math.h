//############################################################
// 文件名: IQ_math.h
// 创建日期: 2017年1月18日
// 作者: XQ
// 功能概述: IQ数学库头文件 
// 该文件仅用于学习使用，未经允许不得用于其他任何用途
// 版权所有：XQ
// DSP/STM32开发板
// 网址: https://shuolidianzi.taobao.com
// 修改日期: 2017/1/23
// 版本：V17.3-1
// 作者QQ: 616264123
// 技术交流QQ群：314306105
//############################################################
   
#ifndef _IQ_math_H          // 防止头文件重复包含
#define _IQ_math_H 

// 包含系统头文件
#include "./SYSTEM/sys/sys.h"

// 基本数学运算宏定义
#define Abs(A)    ((A>=0)?A:-A)           // 计算绝对值
#define Min(A,B)  ((A<=B)?A:B)            // 获取最小值
#define Max(A,B)  ((A>=B)?A:B)            // 获取最大值

// IQ格式定点数运算宏定义
#define _IQ10(A)       (int32_t)((A)<<15)      // 将浮点数转换为Q15格式定点数
#define _IQ(A)         _IQ15(A)                 // 默认使用Q15格式
#define _IQmpy(A,B)    (int32_t)((A*B)>>15)    // Q15格式定点数乘法
#define _IQ10mpy(A,B)  (int32_t)((A*B)>>10)    // Q10格式定点数乘法
#define _IQdiv2(A)     (int32_t)((A)>>1)       // 定点数除以2（右移1位）
#define _IQmpy2(A)     (int32_t)(A<<1)         // 定点数乘以2（左移1位）
#define _IQdiv(A,B)    (int32_t)((A<<15)/B)    // 定点数除法

// 角度象限定义
#define SIN_RAD     0x0300    // 象限掩码，用于确定角度所在象限
#define U0_90       0x0000    // 第一象限（0-90度）
#define U90_180     0x0100    // 第二象限（90-180度）
#define U180_270    0x0200    // 第三象限（180-270度）
#define U270_360    0x0300    // 第四象限（270-360度）

// 正弦余弦计算结构体
typedef struct { 
    int32_t  IQAngle;    // 输入：角度值（Q15格式）
    int32_t  IQSin;      // 输出：正弦值（Q15格式）
    int32_t  IQCos;      // 输出：余弦值（Q15格式）
} IQSin_Cos, *p_IQSin_Cos;

// 正弦余弦结构体默认初始值
#define IQSin_Cos_DEFAULTS  {0, 0, 0}

// 反正切计算结构体
typedef struct { 
    int32_t  Alpha;      // 输入：Alpha轴分量（Q15格式）
    int32_t  Beta;       // 输入：Beta轴分量（Q15格式）
    int32_t  IQTan;      // 输出：正切值（Q15格式）
    int32_t  IQAngle;    // 输出：角度值（Q15格式）
    int32_t  JZIQAngle;  // 输出：校正后的角度值（Q15格式）
} IQAtan, *p_IQAtan;

// 反正切结构体默认初始值
#define IQAtan_DEFAULTS  {0, 0, 0, 0, 0}

// 函数声明
uint32_t IQSqrt(uint32_t M);                      // 计算32位无符号整数的平方根
void IQSin_Cos_Cale(p_IQSin_Cos pV);             // 计算给定角度的正弦余弦值
void IQAtan_Cale(p_IQAtan pV);                   // 计算反正切值（用于坐标转换）
int32_t IQsat(int32_t Uint, int32_t U_max, int32_t U_min);  // 输入值限幅函数

#endif /* _IQ_math_H */

//===========================================================================
// 文件结束
//===========================================================================
