#include "PWM.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

void pwm_init(void)
{
	PWMPOL=0x00; //PWM channel 0-7 outputs are low at the beginning of the period
	PWMPRCLK=0b01110111;	//Clock B & Clock A =Bus clock / 128
	PWMCLK=0x00;	//Clock B & Clock A is the clock source for PWM channel0-7
	//PWME=0xff;	//enabled the channel
}

void pwm_on_off(uint8_t OnOff,uint8_t bit)
{
	switch(OnOff)
	{
		case 0:
			PWME &=~(1<<bit);
			break;
		case 1:
			PWME |=(1<<bit);
			break;	
	}		
}


void pwm_SetPeriodAndDuty(uint8_t channel,uint8_t period,uint8_t duty)
{    
  
  
  switch(channel)
  {
  case 0:
  	PWMPER0= period;   
	PWMDTY0= duty;
	break;	
  case 1:
  	PWMPER1= period;   
	PWMDTY1= duty;
	break;
  case 2:
  	PWMPER2= period;   
	PWMDTY2= duty;
	break;
  case 3:
  	PWMPER3= period;   
	PWMDTY3= duty;
	break;
  case 4:
  	PWMPER4= period;   
	PWMDTY4= duty;
	break;
  case 5:
  	PWMPER5= period;   
	PWMDTY5= duty;
	break;
  case 6:
  	PWMPER6= period;   
	PWMDTY6= duty;
	break;
  case 7:
  	PWMPER7= period;   
	PWMDTY7= duty;
	break;
  }    
}