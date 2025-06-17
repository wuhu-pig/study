#include "pid.h"

// 初始化PID控制器
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, 
              float output_min, float output_max, float integral_max) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output = 0.0f;
    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->integral_max = integral_max;
}

// 更新PID控制器
float PID_Update(PID_Controller *pid, float setpoint, float measurement, float dt) {
    // 计算误差
    float error = setpoint - measurement;
    
    // 比例项
    float proportional = pid->Kp * error;
    
    // 积分项（带抗饱和）
    pid->integral += pid->Ki * error * dt;
    
    // 限制积分项
    if (pid->integral > pid->integral_max) 
        pid->integral = pid->integral_max;
    else if (pid->integral < -pid->integral_max) 
        pid->integral = -pid->integral_max;
    
    // 微分项
    float derivative = pid->Kd * (error - pid->prev_error) / dt;
    pid->prev_error = error;
    
    // 计算输出
    pid->output = proportional + pid->integral + derivative;
    
    // 限制输出
    if (pid->output > pid->output_max) 
        pid->output = pid->output_max;
    else if (pid->output < pid->output_min) 
        pid->output = pid->output_min;
    
    return pid->output;
}