#ifndef __SMO_H
#define __SMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

// 滑模观测器参数结构
typedef struct {
    float Rs;           // 定子电阻 (Ω)
    float Ls;           // 定子电感 (H)
    float Kslf;         // 滑模增益
    float Freq;         // PWM频率 (Hz)
    uint16_t POLES;     // 电机极对数
    float LPF_cutoff;   // 低通滤波器截止频率 (Hz)
    float sat_boundary; // 饱和函数边界值
} SMO_Params;

// 滑模观测器状态结构
typedef struct {
    float Ialpha_est;   // α轴电流估计值
    float Ibeta_est;    // β轴电流估计值
    float EMF_alpha;    // α轴反电动势估计值
    float EMF_beta;     // β轴反电动势估计值
    float theta;        // 转子位置（电角度，弧度）
    float speed;        // 转速（RPM）
    float EMF_alpha_fil; // 滤波后的α轴反电动势
    float EMF_beta_fil;  // 滤波后的β轴反电动势
    float theta_prev;    // 前一次转子位置
} SMO_Handle;

// 初始化滑模观测器
void SMO_Init(SMO_Handle *h);

// 滑模观测器更新
void SMO_Update(SMO_Handle *h, SMO_Params *params, 
                float Ia, float Ib, float Ualpha, float Ubeta);

// 获取转子位置和速度
void SMO_GetPosition(SMO_Handle *h, SMO_Params *params);

// 反电动势低通滤波
void SMO_EMF_Filter(SMO_Handle *h, SMO_Params *params);

// 饱和函数（减少抖振）
float sat(float x, float boundary);

#ifdef __cplusplus
}
#endif

#endif /* __SMO_H */