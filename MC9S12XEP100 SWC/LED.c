#include "LED.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdint.h>
#include "ATD.h"
#include "PWM.h"
#define LED PTP
extern int8_t led_number;

static uint8_t led_status[8]={eLed_OFF,eLed_OFF,eLed_OFF,eLed_OFF,eLed_OFF,eLed_OFF,eLed_OFF,eLed_OFF};
static uint8_t status_output[eLed_StatusMax]={1,1,1,1,1,1};	//初始化输出高电平，灯灭	
static uint8_t timer[eLed_StatusMax]={0,0,0,0,0,0};
static uint8_t AtdTimer=0;
static const uint8_t led_duty[eLed_StatusMax]={25,0,25,12,8,6};
static const uint8_t led_period[eLed_StatusMax]={25-1,255,50-1,25-1,16-1,12-1};
static uint8_t eLed_complex_st=eLed_1HZ;	//复制状态初始1HZ
static uint8_t eLed_complex_st_ct=0;
static volatile uint8_t duty0=20;
static volatile uint8_t duty1=20;
static volatile uint16_t AtdFinalResult0=4095,AtdFinalResult1=4095;



void led_init(void)
{
	/*initialize led*/
	DDRP=0xff;	// set data direction register as output
	LED=0xff;	//unlight the led
}

void led_status_change(int8_t led_number,led_status_t status)
{
	led_status[led_number]=(uint8_t)status;	
}

void led_periodic_task(void)
{

	volatile uint16_t AtdResult0,AtdResult1;
	uint8_t i;
	
	for(i=0;i<eLed_complex;i++)
	{
		timer[i]++;
		if(timer[i]<led_duty[i])
		{
			status_output[i]=0;
		}
		else
		{
			status_output[i]=1;
			if(timer[i]>led_period[i])
			{
				timer[i]=0;
				status_output[i]=0;	
			}	
		}
			
	}
	
	timer[eLed_complex]++;
	switch(eLed_complex_st)
	{
		case eLed_1HZ:
			if(timer[eLed_complex]<led_duty[eLed_1HZ])
			{
				status_output[eLed_complex]=0;
			}
			else
			{
				status_output[eLed_complex]=1;
				if(timer[eLed_complex]>led_period[eLed_1HZ])
				{
					timer[eLed_complex]=0;
					status_output[eLed_complex]=0;
					eLed_complex_st=eLed_2HZ;	
				}
					
			}
		
			break;
		case eLed_2HZ:
			if(timer[eLed_complex]<led_duty[eLed_2HZ])
			{
				status_output[eLed_complex]=0;
			}
			else
			{
				status_output[eLed_complex]=1;
				if(timer[eLed_complex]>led_period[eLed_2HZ])
				{
					timer[eLed_complex]=0;
					status_output[eLed_complex]=0;
					eLed_complex_st_ct++;
					if(2==eLed_complex_st_ct)
					{
						eLed_complex_st=eLed_3HZ;
						eLed_complex_st_ct=0;	
					}	
				}	
			}

			break;
		case eLed_3HZ:
			if(timer[eLed_complex]<led_duty[eLed_3HZ])
			{
				status_output[eLed_complex]=0;
			}
			else
			{
				status_output[eLed_complex]=1;
				if(timer[eLed_complex]>led_period[eLed_3HZ])
				{
					timer[eLed_complex]=0;
					status_output[eLed_complex]=0;
					eLed_complex_st_ct++;
					if(3==eLed_complex_st_ct)
					{
						eLed_complex_st=eLed_1HZ;
						eLed_complex_st_ct=0;	
					}	
				}	
			}
			
			break;
	}
	AtdTimer++;
	if(AtdTimer=5)
	{
		AtdTimer=0;
		AtdResult0=atd_capture(0);
		AtdFinalResult0 = AtdFinalResult0*7/8+AtdResult0/8;   //滤波以消除抖动
		AtdResult1=atd_capture(1);
		AtdFinalResult1= AtdFinalResult1*7/8+AtdResult1/8;   //滤波以消除抖动
		duty0=(uint8_t)((uint32_t)250*AtdFinalResult0/4095);
		duty1=(uint8_t)((uint32_t)250*AtdFinalResult1/4095);
	}
	

	for(i=0;i<8;i++)	//	循环遍历灯的状态
	{
		if(status_output[led_status[i]] == 1)	//the led is OFF
		{
			pwm_on_off(0,i);
			LED |= (1<<i);
		}
		else	//the led is ON
		{
			pwm_on_off(1,i);
			if(i<4)
			{
				pwm_SetPeriodAndDuty(i,250,duty0);	//	controll the led0~3
			}
			else
			{
				pwm_SetPeriodAndDuty(i,250,duty1);	//	controll the led4~8
			}
		}
	}	
}

