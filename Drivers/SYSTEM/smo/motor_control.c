#include "motor_control.h"
#include "foc_math.h"

// 初始化电机控制
void MotorControl_Init(Motor_Control *mc) {
    mc->state = MOTOR_STOPPED;
    mc->start_time = 0;
    mc->open_loop_angle = 0.0f;
    mc->open_loop_freq = 1.0f; // 初始开环频率 (Hz)
    mc->open_loop_voltage = 1.0f; // 初始开环电压
    
    // 初始化SMO参数
    mc->smo_params.Rs = 0.5f;        // 电机电阻 (Ω)
    mc->smo_params.Ls = 0.001f;      // 电机电感 (H)
    mc->smo_params.Kslf = 0.2f;      // 滑模增益
    mc->smo_params.Freq = 20000.0f;  // PWM频率 (Hz)
    mc->smo_params.POLES = 7;        // 电机极对数
    mc->smo_params.LPF_cutoff = 50.0f; // 低通滤波截止频率 (Hz)
    mc->smo_params.sat_boundary = 0.05f; // 饱和函数边界值
    
    // 初始化SMO
    SMO_Init(&mc->smo);
    
    // 初始化PID控制器
    PID_Init(&mc->pid_speed, 0.1f, 0.01f, 0.001f, -10.0f, 10.0f, 1.0f); // 速度环
    PID_Init(&mc->pid_id, 1.0f, 0.1f, 0.0f, -5.0f, 5.0f, 1.0f);         // d轴电流环
    PID_Init(&mc->pid_iq, 1.0f, 0.1f, 0.0f, -5.0f, 5.0f, 1.0f);         // q轴电流环
}

// 电机控制主循环
void MotorControl_Update(Motor_Control *mc, 
                         float Ia, float Ib, float Ic, 
                         float Vdc, uint32_t timestamp) {
    float Ialpha, Ibeta;
    float Id, Iq;
    float Valpha, Vbeta;
    float Ualpha, Ubeta; // 实际施加的电压
    
    // 1. Clarke变换
    Clarke_Transform(Ia, Ib, Ic, &Ialpha, &Ibeta);
    
    switch(mc->state) {
        case MOTOR_STOPPED:
            // 停止状态，输出零PWM
            // ... (设置PWM输出为零)
            break;
            
        case MOTOR_ALIGNMENT:
            // 预定位阶段（固定角度）
            mc->smo.theta = 0.0f; // 固定0度位置
            
            // 设置固定电压
            Valpha = mc->open_loop_voltage;
            Vbeta = 0.0f;
            
            // 检查是否结束预定位
            if(timestamp - mc->start_time > 500) { // 500ms后结束预定位
                mc->state = MOTOR_OPEN_LOOP;
                mc->start_time = timestamp;
            }
            break;
            
        case MOTOR_OPEN_LOOP:
            // 开环启动阶段（VF控制）
            mc->open_loop_angle += mc->open_loop_freq / mc->smo_params.Freq * TWO_PI;
            if(mc->open_loop_angle > TWO_PI) mc->open_loop_angle -= TWO_PI;
            
            // 设置开环电压矢量
            Valpha = mc->open_loop_voltage * cosf(mc->open_loop_angle);
            Vbeta = mc->open_loop_voltage * sinf(mc->open_loop_angle);
            
            // 逐步增加频率和电压
            if(timestamp - mc->start_time > 10) { // 每10ms增加一次
                mc->open_loop_freq += 0.1f; // 频率增加0.1Hz
                mc->open_loop_voltage += 0.01f; // 电压增加0.01
                mc->start_time = timestamp;
            }
            
            // 当频率达到一定值时切换到闭环
            if(mc->open_loop_freq > 5.0f) { // 达到5Hz时切换
                mc->state = MOTOR_CLOSED_LOOP;
                mc->smo.theta_prev = mc->open_loop_angle; // 初始化角度
            }
            break;
            
        case MOTOR_CLOSED_LOOP:
            // 闭环运行（FOC控制）
            
            // 2. Park变换
            Park_Transform(Ialpha, Ibeta, mc->smo.theta, &Id, &Iq);
            
            // 3. 更新SMO观测器（需要实际施加的电压）
            SMO_Update(&mc->smo, &mc->smo_params, Ia, Ib, Ualpha, Ubeta);
            SMO_EMF_Filter(&mc->smo, &mc->smo_params);
            SMO_GetPosition(&mc->smo, &mc->smo_params);
            
            // 4. 速度环PID
            float speed_ref = 1000.0f; // 目标速度1000 RPM
            float Iq_ref = PID_Update(&mc->pid_speed, speed_ref, mc->smo.speed, 
                                     1.0f/mc->smo_params.Freq);
            
            // 5. 电流环PID
            float Id_ref = 0.0f; // d轴电流设为0（最大转矩控制）
            float Vd = PID_Update(&mc->pid_id, Id_ref, Id, 1.0f/mc->smo_params.Freq);
            float Vq = PID_Update(&mc->pid_iq, Iq_ref, Iq, 1.0f/mc->smo_params.Freq);
            
            // 6. 逆Park变换
            Inv_Park_Transform(Vd, Vq, mc->smo.theta, &Valpha, &Vbeta);
            break;
    }
    
    // 7. 生成SVPWM
    float Ta, Tb, Tc;
    SVPWM_Generate(Valpha, Vbeta, Vdc, &Ta, &Tb, &Tc);
    
    // 8. 更新PWM输出（根据硬件实现）
    // ... (设置定时器占空比)
    
    // 保存实际施加的电压用于SMO
    Ualpha = (Ta - 0.5f) * Vdc;
    Ubeta = (Tb - 0.5f) * Vdc * ONE_BY_SQRT3; // 近似计算
}