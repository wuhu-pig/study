#ifndef __PID_H
#define __PID_H

#ifdef __cplusplus
extern "C" {
#endif

// PID控制器结构
typedef struct {
    float Kp;           // 比例增益
    float Ki;           // 积分增益
    float Kd;           // 微分增益
    float integral;     // 积分项
    float prev_error;   // 前一次误差
    float output;       // 输出值
    float output_max;   // 输出上限
    float output_min;   // 输出下限
    float integral_max; // 积分项上限
} PID_Controller;

// 初始化PID控制器
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, 
              float output_min, float output_max, float integral_max);

// 更新PID控制器
float PID_Update(PID_Controller *pid, float setpoint, float measurement, float dt);

#ifdef __cplusplus
}
#endif

#endif /* __PID_H */