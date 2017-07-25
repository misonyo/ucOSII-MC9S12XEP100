#include "KEY.h"
#include "ShuMaGuan.h"
#include "LED.h"
#include "derivative.h"      /* derivative-specific definitions */
#include <stdint.h>

int8_t led_number=7;

void key_init(void)
{
 	/*initialize key*/
	DDRH=0x00;	// set data direction register as input
	PIFH = 0xff;	//对PIFH的每一位写1来清除标志位;
    PIEH = 0xff;	//中断使能寄存器;
    ShuMaGuan_display(0,led_number);
}

void key_task(uint8_t flag)
{

	if(flag & 0x01)//led off
	{
		led_status_change(led_number,eLed_OFF);			
	}
	
	if(flag & 0x02)//led on
	{
		led_status_change(led_number,eLed_ON);	
	}
	if(flag & 0x04)
	{
	 	
        if(led_number>0)
        {
        	led_number--;
        }
        else
        {
        	led_number = 7;
        }
        ShuMaGuan_display(0,led_number); 
	}
	if(flag & 0x08)
	{
		
        led_number++;
        if(led_number>7)
        {
        	led_number=0;
        }
        ShuMaGuan_display(0,led_number);
	}
	if(flag & 0x10)
	{
	 	led_status_change(led_number,eLed_1HZ);
	 		
	}
	if(flag & 0x20)
	{
	 	led_status_change(led_number,eLed_2HZ);
	 		
	}
	if(flag & 0x40)
	{
	 	led_status_change(led_number,eLed_3HZ);
	 	
	}
	if(flag & 0x80)
	{
	 	led_status_change(led_number,eLed_complex);
	}	
}

