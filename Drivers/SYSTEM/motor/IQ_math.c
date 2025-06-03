//############################################################
// FILE: Timer.c
// Created on: 2017年1月11日
// Author: XQ
// summary: Timer
//  定时1秒分频，输出定时IO    
//该文件仅用于学习使用，不能用于其他目的
//版权所有：XQ
//DSP/STM32开发板
//网址: https://shuolidianzi.taobao.com
//修改日期:2017/1/24
//版本：V17.3-1
//Author-QQ: 616264123
//QQ群号：314306105
//############################################################
#include "IQ_math.h"

// 正弦/余弦查找表
// 包含256个点的四分之一正弦波数据
// 数据格式：Q15定点数，范围0x0000-0x7FFE，相当于0-0.9999
const int16_t IQSin_Cos_Table[256]={\
0x0000,0x00C9,0x0192,0x025B,0x0324,0x03ED,0x04B6,0x057F,\
0x0648,0x0711,0x07D9,0x08A2,0x096A,0x0A33,0x0AFB,0x0BC4,\
0x0C8C,0x0D54,0x0E1C,0x0EE3,0x0FAB,0x1072,0x113A,0x1201,\
0x12C8,0x138F,0x1455,0x151C,0x15E2,0x16A8,0x176E,0x1833,\
0x18F9,0x19BE,0x1A82,0x1B47,0x1C0B,0x1CCF,0x1D93,0x1E57,\
0x1F1A,0x1FDD,0x209F,0x2161,0x2223,0x22E5,0x23A6,0x2467,\
0x2528,0x25E8,0x26A8,0x2767,0x2826,0x28E5,0x29A3,0x2A61,\
0x2B1F,0x2BDC,0x2C99,0x2D55,0x2E11,0x2ECC,0x2F87,0x3041,\
0x30FB,0x31B5,0x326E,0x3326,0x33DF,0x3496,0x354D,0x3604,\
0x36BA,0x376F,0x3824,0x38D9,0x398C,0x3A40,0x3AF2,0x3BA5,\
0x3C56,0x3D07,0x3DB8,0x3E68,0x3F17,0x3FC5,0x4073,0x4121,\
0x41CE,0x427A,0x4325,0x43D0,0x447A,0x4524,0x45CD,0x4675,\
0x471C,0x47C3,0x4869,0x490F,0x49B4,0x4A58,0x4AFB,0x4B9D,\
0x4C3F,0x4CE0,0x4D81,0x4E20,0x4EBF,0x4F5D,0x4FFB,0x5097,\
0x5133,0x51CE,0x5268,0x5302,0x539B,0x5432,0x54C9,0x5560,\
0x55F5,0x568A,0x571D,0x57B0,0x5842,0x58D3,0x5964,0x59F3,\
0x5A82,0x5B0F,0x5B9C,0x5C28,0x5CB3,0x5D3E,0x5DC7,0x5E4F,\
0x5ED7,0x5F5D,0x5FE3,0x6068,0x60EB,0x616E,0x61F0,0x6271,\
0x62F1,0x6370,0x63EE,0x646C,0x64E8,0x6563,0x65DD,0x6656,\
0x66CF,0x6746,0x67BC,0x6832,0x68A6,0x6919,0x698B,0x69FD,\
0x6A6D,0x6ADC,0x6B4A,0x6BB7,0x6C23,0x6C8E,0x6CF8,0x6D61,\
0x6DC9,0x6E30,0x6E96,0x6EFB,0x6F5E,0x6FC1,0x7022,0x7083,\
0x70E2,0x7140,0x719D,0x71F9,0x7254,0x72AE,0x7307,0x735E,\
0x73B5,0x740A,0x745F,0x74B2,0x7504,0x7555,0x75A5,0x75F3,\
0x7641,0x768D,0x76D8,0x7722,0x776B,0x77B3,0x77FA,0x783F,\
0x7884,0x78C7,0x7909,0x794A,0x7989,0x79C8,0x7A05,0x7A41,\
0x7A7C,0x7AB6,0x7AEE,0x7B26,0x7B5C,0x7B91,0x7BC5,0x7BF8,\
0x7C29,0x7C59,0x7C88,0x7CB6,0x7CE3,0x7D0E,0x7D39,0x7D62,\
0x7D89,0x7DB0,0x7DD5,0x7DFA,0x7E1D,0x7E3E,0x7E5F,0x7E7E,\
0x7E9C,0x7EB9,0x7ED5,0x7EEF,0x7F09,0x7F21,0x7F37,0x7F4D,\
0x7F61,0x7F74,0x7F86,0x7F97,0x7FA6,0x7FB4,0x7FC1,0x7FCD,\
0x7FD8,0x7FE1,0x7FE9,0x7FF0,0x7FF5,0x7FF9,0x7FFD,0x7FFE};

// 计算给定角度的正弦和余弦值
// 参数：pV - 指向IQSin_Cos结构体的指针，包含输入角度和输出结果
// 说明：使用查表法计算，将360度分为四个象限分别处理
void IQSin_Cos_Cale(p_IQSin_Cos pV)  
{
    uint16_t hindex;
    // 将输入角度(-32768~32767)转换为查表索引
    hindex = (uint16_t)pV->IQAngle;    // 将有符号角度转换为无符号索引
    hindex >>= 6;                       // 将65536个点映射到1024个点(256*4个象限)

    // 根据象限选择不同的计算方法
    switch (hindex & SIN_RAD)           // 使用掩码确定象限
    {
        case U0_90:     // 第一象限 0-90度
            pV->IQSin = IQSin_Cos_Table[(uint8_t)(hindex)];                    // 直接查表得到正弦值
            pV->IQCos = IQSin_Cos_Table[(uint8_t)(0xFF-(uint8_t)(hindex))];   // 余弦值为反向查表
            break;
    
        case U90_180:   // 第二象限 90-180度
            pV->IQSin = IQSin_Cos_Table[(uint8_t)(0xFF-(uint8_t)(hindex))];   // 正弦值为反向查表
            pV->IQCos = -IQSin_Cos_Table[(uint8_t)(hindex)];                  // 余弦值为负的正向查表
            break;
    
        case U180_270:  // 第三象限 180-270度
            pV->IQSin = -IQSin_Cos_Table[(uint8_t)(hindex)];                  // 正弦值为负的正向查表
            pV->IQCos = -IQSin_Cos_Table[(uint8_t)(0xFF-(uint8_t)(hindex))]; // 余弦值为负的反向查表
            break;
    
        case U270_360:  // 第四象限 270-360度
            pV->IQSin = -IQSin_Cos_Table[(uint8_t)(0xFF-(uint8_t)(hindex))]; // 正弦值为负的反向查表
            pV->IQCos = IQSin_Cos_Table[(uint8_t)(hindex)];                   // 余弦值为正向查表
            break;

        default:
            break;
    } 
} 

// 限幅函数
// 参数：
//   Uint  - 输入值
//   U_max - 上限值
//   U_min - 下限值
// 返回：限幅后的结果
int32_t IQsat(int32_t Uint, int32_t U_max, int32_t U_min) 
{
    int32_t Uout; 
    if(Uint <= U_min)           // 输入小于下限
        Uout = U_min;           // 限制为下限值
    else if(Uint >= U_max)      // 输入大于上限
        Uout = U_max;           // 限制为上限值
    else
        Uout = Uint;            // 在范围内，直接输出
 
    return Uout; 
}

// 计算32位无符号整数的平方根
// 参数：M - 输入值
// 返回：平方根结果
// 说明：使用迭代法计算平方根
uint32_t IQSqrt(uint32_t M)
{
    uint32_t N, i, tmp, ttp;    // 定义计算所需变量
    
    if(M == 0)                  // 如果输入为0
        return 0;               // 直接返回0
    
    N = 0;                      // 初始化结果

    tmp = (M >> 30);            // 获取输入的最高两位
    M <<= 2;                    // 输入左移2位
    
    // 处理最高位
    if(tmp > 1)
    {
        N++;                    // 结果加1
        tmp -= N;               // 更新余数
    }
    
    // 逐位计算平方根
    for(i = 15; i > 0; i--)    // 15次迭代
    {
        N <<= 1;                // 结果左移1位
        
        tmp <<= 2;              // 余数左移2位
        tmp += (M >> 30);       // 加入新的2位
        
        ttp = N;                // 保存当前结果
        ttp = (ttp << 1) + 1;   // 计算下一位的判断值
        
        M <<= 2;                // 输入值左移2位，准备下次迭代
        if(tmp >= ttp)          // 如果余数足够大
        {
            tmp -= ttp;         // 减去判断值
            N++;                // 结果加1
        }
    }
    
    return N;                   // 返回平方根结果
}

//===========================================================================
// No more.
//===========================================================================
