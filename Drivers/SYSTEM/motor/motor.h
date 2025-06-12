#ifndef  MOTOR_H
#define  MOTOR_H

//#include "IQ_math.h"
//#include "PI_Cale.h"
//#include "Axis_transform.h"
//#include "IQ_math.h"
//#include "Svpwm_dq.h"
#include "status.h"
#include "adc.h" 
extern ADCSamp  ADCSampPare;

#define motorpoles 7
//#define PI 3.1415926535897932384626433832795
extern float voltage_power_supply;
extern float shaft_angle,open_loop_timestamp;
extern float zero_electric_angle,Ualpha,Ubeta,Ua,Ub,Uc,dc_a,dc_b,dc_c;
//#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

extern status_t Offset_CurrentReading(void);
extern void motor_init(void);
extern void ADC_Sample(void);
extern void motor_foc_algorithm_main(void);
extern void motor_bldc_algorithm_main(void);
#endif
