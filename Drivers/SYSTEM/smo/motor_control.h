#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "smo.h"
#include "pid.h"

// 电机控制状态机
typedef enum {
    MOTOR_STOPPED,      // 电机停止
    MOTOR_ALIGNMENT,    // 预定位
    MOTOR_OPEN_LOOP,    // 开环启动
    MOTOR_CLOSED_LOOP   // 闭环运行
} Motor_State;

// 电机控制结构
typedef struct {
    Motor_State state;          // 当前状态
    uint32_t start_time;        // 启动时间
    float open_loop_angle;      // 开环角度
    float open_loop_freq;       // 开环频率
    float open_loop_voltage;    // 开环电压
    SMO_Handle smo;             // 滑模观测器
    SMO_Params smo_params;      // SMO参数
    PID_Controller pid_speed;   // 速度环PID
    PID_Controller pid_id;      // d轴电流环PID
    PID_Controller pid_iq;      // q轴电流环PID
} Motor_Control;

// 初始化电机控制
void MotorControl_Init(Motor_Control *mc);

// 电机控制主循环
void MotorControl_Update(Motor_Control *mc, 
                         float Ia, float Ib, float Ic, 
                         float Vdc, uint32_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_CONTROL_H */