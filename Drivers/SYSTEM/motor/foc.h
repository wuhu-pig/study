#ifndef __FOC_H
#define __FOC_H

#include "sys.h"

#define PI          3.14159265358979323846f
#define TWO_PI      (2.0f * PI)
#define SQRT3       1.73205080757f
#define ONE_BY_SQRT3 0.57735026919f
#define SQRT3_BY_2  0.86602540378f


// 电流采样结构体
typedef struct {
    float ia;
    float ib;
    float ic;
} PhaseCurrent_t;

// 电压向量结构体
typedef struct {
    float vd;
    float vq;
} VoltageDQ_t;

// 电流向量结构体
typedef struct {
    float id;
    float iq;
} CurrentDQ_t;

// 编码器反馈结构体
typedef struct {
    float angle_elec;  // 电角度
    float speed_rpm;   // 转速（可选）
} MotorFeedback_t;

// PI 控制器结构体
typedef struct {
    float kp;
    float ki;
    float integral;
    float output_limit;
} PI_Controller_t;

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


// 电机控制状态机
typedef enum {
    MOTOR_STOPPED,      // 电机停止
    MOTOR_ALIGNMENT,    // 预定位
    MOTOR_OPEN_LOOP,    // 开环启动
    MOTOR_CLOSED_LOOP   // 闭环运行
} Motor_State;

typedef struct{
	PhaseCurrent_t	phasecurrent;
	VoltageDQ_t 		voltage;
	CurrentDQ_t 		dqcurrent;
	SMO_Params  		smoparams;
	PI_Controller_t pid;
}mootor; 

#endif
