#include "foc.h"
#include "math.h"
#include "atim.h"

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
	my_motor.control.targetspeed=1;
	my_motor.control.voltage0.vq=Vref/2;
	//电机状态初始化
	my_motor.state=MOTOR_STOPPED;
}
void speed_rampup(void)
{
	if(my_motor.control.speed<=my_motor.control.maxspeed)
	{
		if(my_motor.control.speed<my_motor.control.targetspeed)
			{
				my_motor.control.speed+=RAMPSTEP;
			}
	}
}
	
// Park逆变换：两相旋转（dq）→两相静止（αβ）[theta单位为弧度]
void inverse_park_transform(float d, float q, float theta, float *alpha, float *beta) 
{
    float cos_theta = cos(theta);
    float sin_theta = sin(theta);
    *alpha = d * cos_theta - q * sin_theta;
    *beta  = d * sin_theta + q * cos_theta;
}

// Clarke逆变换：两相静止（αβ）→三相静止（abc）（假设零序分量为0）
void inverse_clarke_transform(float alpha, float beta, float *a, float *b, float *c) 
{
    *a = alpha+Vrefby2;
    *b = -0.5 * alpha + SQRT3_BY_2 * beta+Vrefby2;
    *c = -0.5 * alpha - SQRT3_BY_2 * beta+Vrefby2;
}

float _normalizeAngle(float angle)
{
    float a = fmod(angle, 2*PI);   //取余，将角度限制在一个周期内，超出部分舍去
    return a >= 0 ? a : (a + 2*PI);  
}

void phasesetpwm(float *a, float *b, float *c,uint16_t *ccra,uint16_t *ccrb,uint16_t *ccrc)
{
	*ccra=_constrain(*a*1000/Vref, 0, 1000 );
	*ccrb=_constrain(*b*1000/Vref, 0, 1000 );
	*ccrc=_constrain(*c*1000/Vref, 0, 1000 );
	atim_timx_cplm_pwm_set(*ccra*1000,1);
	atim_timx_cplm_pwm_set(*ccrb*1000,2);
	atim_timx_cplm_pwm_set(*ccrc*1000,3);
}
void foc_main(void)
{
	speed_rampup();//速度rampup
	my_motor.control.speed_el=my_motor.control.speed*my_motor.params.Poles;//得到电气角速度
	my_motor.control.angle_el=_normalizeAngle(my_motor.control.speed_el*Ts+my_motor.control.angle_el);//得到弧度
	inverse_park_transform(0,my_motor.control.voltage0.vq,my_motor.control.angle_el,(float *)&my_motor.control.voltage1.valpha,(float *)&my_motor.control.voltage1.vbeta);
	inverse_clarke_transform(my_motor.control.voltage1.valpha,my_motor.control.voltage1.vbeta,(float *)&my_motor.control.voltage2.va,(float *)&my_motor.control.voltage2.vb,(float *)&my_motor.control.voltage2.vc);
	phasesetpwm((float *)&my_motor.control.voltage2.va,(float *)&my_motor.control.voltage2.vb,(float *)&my_motor.control.voltage2.vc,(uint16_t  *)&my_motor.control.CCR.ccra,(uint16_t  *)&my_motor.control.CCR.ccrb,(uint16_t  *)&my_motor.control.CCR.ccrc);
}