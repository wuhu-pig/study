/********************** 头文件包含 ***************************/
#include "./SYSTEM/sys/sys.h"
#include "math.h"
#include "motor.h"
#include "atim.h"
#include "adc.h"
#include "foc.h"
/********************** 变      量 ***************************/
float voltage_power_supply=12;
float shaft_angle=0,open_loop_timestamp=0;
float zero_electric_angle=0,Ualpha,Ubeta=0,Ua=0,Ub=0,Uc=0,dc_a=0,dc_b=0,dc_c=0;
ADCSamp  ADCSampPare=ADCSamp_DEFAULTS;
//CLARKE       ClarkeI= CLARKE_DEFAULTS;
//PARK         ParkI=PARK_DEFAULTS;
//IPARK        IparkU=IPARK_DEFAULTS;
//SVPWM        Svpwmdq=SVPWM_DEFAULTS;
//IQSin_Cos    AngleSin_Cos=IQSin_Cos_DEFAULTS ;
//IQAtan       IQAtan_Pare=IQAtan_DEFAULTS;
/********************** 函      数 ***************************/ 
/****************************************************************************
 * 函    数：status_t Offset_CurrentReading(void)
 * 功    能：电流采样校准
 * 输入参数：无 
 * 输出参数：无
 * 说    明：无
 * 调用方法：无
 ****************************************************************************/  
status_t Offset_CurrentReading(void)
{
    uint16_t i,j;
    uint16_t adcx[6];
    uint32_t sum;
    status_t retval=STATUS_ERROR;
    if (g_adc_dma_sta == 1)  //等待DMA传输完成
    {
        for(j = 0; j < 5; j++)  /* 遍历4个通道 */
        {
            sum = 0; /* 清零 */
            for (i = 0; i < ADC_DMA_BUF_SIZE / 6; i++)  /* 每个通道采集到50个数据,这里取10次累加 */
            {
                sum += g_adc_dma_buf[(6 * i) + j];  /* 不同通道的转换结果累加 */
            }
            adcx[j] = sum / (ADC_DMA_BUF_SIZE / 6);    /* 取平均值 */
        }
        g_adc_dma_sta = 0;  /* 清除DMA采集完成状态标志 */
        //adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动下一次ADC DMA采集 */
        
        
        ADCSampPare.OffsetPhaseW_Curr = adcx[0];
        ADCSampPare.OffsetPhaseV_Curr = adcx[1];
        ADCSampPare.OffsetPhaseU_Curr = adcx[2]; 
				ADCSampPare.OffsetBUS_Curr=adcx[3];	
				ADCSampPare.OffsetEA_Curr=adcx[4];				
        retval=STATUS_SUCCESS;
    }
    
    return retval;
}

void ADC_Sample(void)
{
    uint16_t i,j;
    uint32_t sum;
    uint16_t adcx[6];
    if (g_adc_dma_sta == 1)  //等待DMA传输完成
    {
        for(j = 0; j < 5; j++)  /* 遍历4个通道 */
        {
            sum = 0; /* 清零 */
            for (i = 0; i < ADC_DMA_BUF_SIZE / 6; i++)  /* 每个通道采集到50个数据,这里取10次累加 */
            {
                sum += g_adc_dma_buf[(6 * i) + j];  /* 不同通道的转换结果累加 */
            }
            adcx[j] = sum / (ADC_DMA_BUF_SIZE / 6);    /* 取平均值 */
        }
        g_adc_dma_sta = 0;  /* 清除DMA采集完成状态标志 */
//        adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动下一次ADC DMA采集 */     
    }
     
    ADCSampPare.PhaseW_Curr  = (adcx[0]-ADCSampPare.OffsetPhaseW_Curr);     
    ADCSampPare.PhaseV_Curr  = (adcx[1]-ADCSampPare.OffsetPhaseV_Curr); 
    ADCSampPare.PhaseU_Curr =(adcx[2]-ADCSampPare.OffsetPhaseU_Curr);
		ADCSampPare.BUS_Curr  = (adcx[3]-ADCSampPare.OffsetBUS_Curr);	
		ADCSampPare.EA_Curr  = (adcx[3]-ADCSampPare.OffsetEA_Curr);		
		//printf("d:%d,%d,%d,%d\n",ADCSampPare.BUS_Curr,ADCSampPare.PhaseW_Curr,ADCSampPare.PhaseV_Curr,ADCSampPare.PhaseU_Curr);
    //ADCSampPare.BUS_Voltage   = g_adc_dma_buf[4]; 
}

void motor_init(void)
{
    //PI_Pare_init();           //PID参数初始化
    Offset_CurrentReading();   //电流采样校准
}

//float _normalizeAngle(float angle){
//    float a = fmod(angle, 2*PI);   //取余，将角度限制在一个周期内，超出部分舍去
//    return a >= 0 ? a : (a + 2*PI);  
//}

// 电角度计算
float _electricalAngle(float shaft_angle, int pole_pairs) {
    return (shaft_angle * pole_pairs);
}

// 设置PWM输出占空比
void setPwm(float Ua, float Ub, float Uc) {
    // 计算占空比
    // 限制占空比从0到1
    dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
    dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
    dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );
    
    //printf("d: %f, %f, %f\n",dc_a*4200,dc_b*4200,dc_c*4200);
//    //写入PWM到PWM 0 1 2 通道
//    atim_timx_cplm_pwm_set((int)(dc_a*1000),100,1);
//    atim_timx_cplm_pwm_set((int)(dc_b*1000),100,2);
//    atim_timx_cplm_pwm_set((int)(dc_c*1000),100,3);
}

void setPhaseVoltage(float Uq,float Ud, float angle_el) 
{
    //angle_el = _normalizeAngle(angle_el + zero_electric_angle);
    // 逆Park变换
    Ualpha =  -Uq*sin(angle_el); 
    Ubeta =   Uq*cos(angle_el); 

    // 相电压转换
    Ua = Ualpha + voltage_power_supply/2;
    Ub = (sqrt(3)*Ubeta-Ualpha)/2 + voltage_power_supply/2;
    Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + voltage_power_supply/2;
    setPwm(Ua,Ub,Uc);
}

// 添加软启动
float ramp_voltage = 0;
float target_voltage = 12/3;
float voltage_step = 0.001;
volatile float angle_step = 0.0005;  // 开始时用较小的步进
volatile float target_angle_step = 0.0008;  // 开始时用较小的步进
void motor_foc_algorithm_main()
{
    // 电压缓慢上升
    if(ramp_voltage < target_voltage) {
        ramp_voltage += voltage_step;
    }
    
    // 速度也可以缓慢增加
    
    if(ramp_voltage > target_voltage * 0.5) {
        //angle_step = 0.0005;    // 电压达到一定值后增加速度
				if(angle_step<target_angle_step)
				{
					angle_step+=0.00001;
				}else{
					angle_step=target_angle_step;
				}				
    }
    
    //shaft_angle = _normalizeAngle(shaft_angle + angle_step);
    setPhaseVoltage(ramp_voltage, 0, _electricalAngle(shaft_angle, motorpoles));
}

void motor_bldc_algorithm_main()
{
    
}
