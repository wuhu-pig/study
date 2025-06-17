#include "smo.h"
#include "foc_math.h" // 包含数学工具函数

// 初始化滑模观测器
void SMO_Init(SMO_Handle *h) {
    h->Ialpha_est = 0.0f;
    h->Ibeta_est = 0.0f;
    h->EMF_alpha = 0.0f;
    h->EMF_beta = 0.0f;
    h->theta = 0.0f;
    h->speed = 0.0f;
    h->EMF_alpha_fil = 0.0f;
    h->EMF_beta_fil = 0.0f;
    h->theta_prev = 0.0f;
}

// 饱和函数（减少抖振）
float sat(float x, float boundary) {
    if (x > boundary) 
        return 1.0f;
    else if (x < -boundary) 
        return -1.0f;
    else 
        return x / boundary;
}

// 滑模观测器更新
void SMO_Update(SMO_Handle *h, SMO_Params *params, 
                float Ia, float Ib, float Ualpha, float Ubeta) {
    // 1. 计算电流误差
    float e_alpha = Ia - h->Ialpha_est;
    float e_beta = Ib - h->Ibeta_est;
    
    // 2. 滑模控制量（使用饱和函数减少抖振）
    float sat_alpha = sat(e_alpha, params->sat_boundary);
    float sat_beta = sat(e_beta, params->sat_boundary);
    
    // 3. 反电动势估计
    h->EMF_alpha = params->Kslf * sat_alpha;
    h->EMF_beta = params->Kslf * sat_beta;
    
    // 4. 电流观测器更新（离散化欧拉法）
    float Ts = 1.0f / params->Freq;  // 控制周期
    
    h->Ialpha_est += Ts * 
        ( (Ualpha - params->Rs * h->Ialpha_est - h->EMF_alpha) / params->Ls );
    
    h->Ibeta_est += Ts * 
        ( (Ubeta - params->Rs * h->Ibeta_est - h->EMF_beta) / params->Ls );
}

// 反电动势低通滤波
void SMO_EMF_Filter(SMO_Handle *h, SMO_Params *params) {
    static float EMF_alpha_prev = 0;
    static float EMF_beta_prev = 0;
    
    // 计算滤波系数 α = 2πf_cutoff * T
    float alpha = 2 * PI * params->LPF_cutoff / params->Freq;
    
    // 限制滤波系数在0-1之间
    if(alpha > 1.0f) alpha = 1.0f;
    if(alpha < 0.0f) alpha = 0.0f;
    
    // 应用一阶低通滤波器
    h->EMF_alpha_fil = alpha * h->EMF_alpha + (1 - alpha) * EMF_alpha_prev;
    h->EMF_beta_fil = alpha * h->EMF_beta + (1 - alpha) * EMF_beta_prev;
    
    // 保存当前值用于下次滤波
    EMF_alpha_prev = h->EMF_alpha_fil;
    EMF_beta_prev = h->EMF_beta_fil;
}

// 获取转子位置和速度
void SMO_GetPosition(SMO_Handle *h, SMO_Params *params) {
    // 1. 使用反正切计算转子位置（电角度）
    // 注意：使用滤波后的反电动势
    h->theta = fast_atan2(-h->EMF_alpha_fil, h->EMF_beta_fil);
    
    // 2. 规范化角度到[0, 2π]
    if(h->theta < 0) 
        h->theta += 2 * PI;
    
    // 3. 速度估计（后向差分法）
    // 计算电角速度（rad/s）: ω_e = Δθ / Δt
    float elec_omega = (h->theta - h->theta_prev) * params->Freq;
    
    // 转换为机械转速（RPM）: RPM = (ω_e * 60) / (2π * 极对数)
    h->speed = elec_omega * 60.0f / (2 * PI * params->POLES);
    
    // 4. 更新角度记忆值
    h->theta_prev = h->theta;
}