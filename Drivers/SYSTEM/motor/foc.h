#ifndef __FOC_H
#define __FOC_H

#include "sys.h"

#define PI          	3.14159265358979323846f
#define TWO_PI     	 	(2.0f * PI)
#define SQRT3       	1.73205080757f
#define ONE_BY_SQRT3 	0.57735026919f
#define SQRT3_BY_2  	0.86602540378f

#define RAMPSTEP 			0.01
#define MAXSPEED 			5					//单位 rad/s  2*2pi/60=2*2*180/60=12度/s
#define Ts 						0.001			//1ms
#define Vref 					12				//电源电压12v
#define Vrefby2 			6					//电源电压12v

#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

// 电流采样结构体
typedef struct {
    float ia;
    float ib;
    float ic;
} PhaseCurrent_t;


// 电流采样结构体
typedef struct {
    float va;
    float vb;
    float vc;
} PhaseVoltage_t;

// 电流采样结构体
typedef struct {
    uint16_t ccra;
    uint16_t ccrb;
    uint16_t ccrc;
} PhaseCcr_t;

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

// 电压向量结构体
typedef struct {
    float valpha;
    float vbeta;
} Voltageabeta_t;

// 电流向量结构体
typedef struct {
    float ialpha;
    float ibeta;
} Currentabeta_t;

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
	//方便查找逐个列出
		uint16_t Poles;     	// 电机极对数
		uint16_t KV;					//KV值
		float Maxcurrent;			//最大可承受电流
		float Maxvoltage;			//最大电压
		float Phaseresitance;
		float Wireresistance;
		float WireLs;
		float	PhaseLs;
	
    float Rs;           	// 定子电阻 (Ω)
    float Ls;          	 	// 定子电感 (H)
    float Kslf;        	 	// 滑模增益
    float Freq;         	// PWM频率 (Hz)
    float LPF_cutoff;   	// 低通滤波器截止频率 (Hz)
    float sat_boundary; 	// 饱和函数边界值
} Motor_Params;

// 电机控制状态机
typedef enum {
    MOTOR_STOPPED,      // 电机停止
    MOTOR_ALIGNMENT,    // 预定位
    MOTOR_OPEN_LOOP,    // 开环启动
    MOTOR_CLOSED_LOOP   // 闭环运行
} Motor_State;

// 电机控制状态机
typedef struct {
	float targetspeed;
	float targetposition;
	float targetcurrent;
	float speed;
	float speed_el;
	float angle_el;
	float position;
	VoltageDQ_t voltage0;
	Voltageabeta_t voltage1;
	PhaseVoltage_t voltage2;
	PhaseCcr_t  CCR;
	float current;
	float maxcurrent;
 	float maxspeed;
} Motor_Control;

typedef struct{
	PhaseCurrent_t	phasecurrent;
	VoltageDQ_t 		voltage;
	CurrentDQ_t 		dqcurrent;
	Motor_Params  	params;
	PI_Controller_t pid;
	Motor_State  state;
	Motor_Control control;
}Motor_t; 

extern Motor_t my_motor;
extern void foc_init(void);
extern void foc_main(void);
#endif
