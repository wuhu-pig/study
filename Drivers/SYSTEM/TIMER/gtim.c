#include "gtim.h"
#include "led.h"
#include "motor.h"
TaskTime   TaskTimePare=TaskTime_DEFAULTS;

void CLEAR_flag(void)
{
	TaskTimePare.Tim1ms_flag=0;
	TaskTimePare.Tim10ms_flag=0;
	TaskTimePare.Tim100ms_flag=0;
	TaskTimePare.Tim500ms_flag=0;
	TaskTimePare.Tim1s_flag=0;
	TaskTimePare.Tim1min_flag=0;
}
void RunSystimer(void)
{
 if(TaskTimePare.IntClock_1ms==1)
    {          
			TaskTimePare.IntClock_1ms=0;
			TaskTimePare.Tim10ms_flag = 1;
			if(++TaskTimePare.Tim10ms_count >=10)
	   {
	    TaskTimePare.Tim10ms_count=0;
	    TaskTimePare.Tim10ms_flag=1;
	   }
    }
		   if(TaskTimePare.Tim10ms_flag==1)
	   {		   		 
	    if(++TaskTimePare.Tim100ms_count >=10)
	    {	      
	       TaskTimePare.Tim100ms_count=0;
	       TaskTimePare.Tim100ms_flag=1;
      }
	   }
       if(TaskTimePare.Tim100ms_flag==1)
	   {		   		 
	    if(++TaskTimePare.Tim500ms_count >=5)
	    {	      
	       TaskTimePare.Tim500ms_count=0;
	       TaskTimePare.Tim500ms_flag=1;
      }
	   }
        if(TaskTimePare.Tim500ms_flag==1)
	   {            
	    if(++TaskTimePare.Tim1s_count >=2)
	    {	               
	       TaskTimePare.Tim1s_count=0;
	       TaskTimePare.Tim1s_flag=1;
      }
	   }
      if(TaskTimePare.Tim1s_flag == 1)
	   {      
			if(++TaskTimePare.Tim10s_count >=10)
	    {
				TaskTimePare.Tim10s_count = 0;
				TaskTimePare.Tim10s_flag = 1;
	    }
	   }
  
      if(TaskTimePare.Tim10s_flag == 1)
	   {                   
				if(++TaskTimePare.Tim1min_count >=6)
	    {
				TaskTimePare.Tim1min_count = 0;
				TaskTimePare.Tim1min_flag = 1;
	    }
	   }
}
/**
 * @brief       通用定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_INT_IRQHandler(void)
{ 
    if (GTIM_TIMX_INT->SR & 0X0001)   /* 溢出中断 */
    {
				GTIM_TIMX_INT->SR &= ~(1 << 0); /* 清除中断标志位 */
        
				TaskTimePare.IntClock_1ms=1;
//				LED1_TOGGLE();
				//motor_foc_algorithm_main();
//				ADC_Sample();
				
				TaskTimePare.Tim1ms_flag=1;
    }

} 

/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();
    GTIM_TIMX_INT->ARR = arr;           /* 设定计数器自动重装值 */
    GTIM_TIMX_INT->PSC = psc;           /* 设置预分频器  */
    GTIM_TIMX_INT->DIER |= 1 << 0;      /* 允许更新中断 */
    GTIM_TIMX_INT->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, GTIM_TIMX_INT_IRQn, 2); /* 抢占1，子优先级3，组2 */
}










