#include "foc.h"
#include "math.h"
Motor_t my_motor;

void foc_init(void)
{
	//参数初始化
	my_motor.params.Poles=7;
	my_motor.params.KV=220;
	my_motor.params.Maxcurrent=20;//最大电流
	my_motor.params.Maxvoltage=5.1;//最大电压
	my_motor.params.Phaseresitance=2.3;//相电阻
	my_motor.params.Wireresistance=5.1;//线电阻
	my_motor.params.WireLs=2.8;//线电感
	my_motor.params.PhaseLs=0.86;//相电感	
	my_motor.params.Kslf=0.01;//滑模增益	
	my_motor.params.Freq=10000;//PWM频率
	my_motor.params.LPF_cutoff=1000;//低通滤波器截止频率
	my_motor.params.sat_boundary=10;//饱和函数边界值
	my_motor.params.Ls=my_motor.params.PhaseLs;//定子电感
	my_motor.params.Rs=my_motor.params.Phaseresitance;//定子电阻

	//PID参数初始化	
	my_motor.pid.kp=0.1;//比例系数
	my_motor.pid.ki=0.01;//积分系数	
	my_motor.pid.integral=0;//积分值
	my_motor.pid.output_limit=100;//输出限幅	
	
	my_motor.control.maxspeed=MAXSPEED;
	//电机状态初始化
	my_motor.state=MOTOR_STOPPED;
}
void speed_rampup(void)
{
	if(my_motor.control.speed<=my_motor.control.maxspeed)
	{
		if(my_motor.control.speed<=my_motor.control.targetspeed)
			{
				my_motor.control.speed+=RAMPSTEP;
			}
	}else
	{
		my_motor.control.speed=my_motor.control.maxspeed;
	}	
}
	
// Park逆变换：两相旋转（dq）→两相静止（αβ）[theta单位为弧度]
void inverse_park_transform(float d, float q, float theta, float *alpha, float *beta) {
    float cos_theta = cos(theta);
    float sin_theta = sin(theta);
    *alpha = d * cos_theta - q * sin_theta;
    *beta  = d * sin_theta + q * cos_theta;
}

// Clarke逆变换：两相静止（αβ）→三相静止（abc）（假设零序分量为0）
void inverse_clarke_transform(float alpha, float beta, float *a, float *b, float *c) {
    *a = alpha;
    *b = -0.5 * alpha + SQRT3_BY_2 * beta;
    *c = -0.5 * alpha - SQRT3_BY_2 * beta;
}

void foc_main(void)
{
	speed_rampup();//速度rampup
	my_motor.control.speed_el=my_motor.control.speed*my_motor.params.Poles;//得到电气角速度
	my_motor.control.angle_el=my_motor.control.speed_el*Ts;//得到角度
	inverse_park_transform(0,Vref/3,my_motor.control.angle_el,(float *)&my_motor.control.voltage1.valpha,(float *)&my_motor.control.voltage1.vbeta);
	inverse_clarke_transform(my_motor.control.voltage1.valpha,my_motor.control.voltage1.vbeta,(float *)&my_motor.control.voltage2.va,(float *)&my_motor.control.voltage2.vb,(float *)&my_motor.control.voltage2.vc);
	
}