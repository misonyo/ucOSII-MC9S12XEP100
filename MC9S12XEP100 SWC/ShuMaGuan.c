#include "ShuMaGuan.h"
#include "derivative.h"      /* derivative-specific definitions */
#include <stdint.h>
#define DISPLAY_NUMBER PORTB

uint8_t ones,tens,hundreds,thousands,DisplayUnit=1;


static const uint8_t shuma[20]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,       //0~9对应的段码
                0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};      //0~9后加小数点对应的段码

void ShuMaGuan_init()
{
	/*initialize ShuMaGuan*/
	DDRK=0x0f;	// set data direction register as output
	PORTK=0x00;	//enable the leds0-3
	DDRB=0xff;	// set data direction register as output
}

void ShuMaGuan_display(uint8_t DisplayUnit,uint8_t number)
{
	PORTK |=(1<<DisplayUnit);
	DISPLAY_NUMBER=shuma[number];
}

void ShuMaGuan_ScanDisplayNumber_split(uint8_t number)
{
	thousands = number/1000;
	hundreds = (number%1000)/100;
	tens = (number%100)/10;
	ones = number%10;
	thousands = thousands+10;         //加小数点	
} 

void ShuMaGuan_ScanDisplayNumber(void)
{
	
	switch(DisplayUnit)
  {
    case 1:
      PORTK_PK3= 1;
      PORTK_PK2= 0;
      PORTK_PK1= 0;
      PORTK_PK0= 0;
      DISPLAY_NUMBER=shuma[thousands];
      break;

    case 2:
      PORTK_PK3= 0;
      PORTK_PK2= 1;
      PORTK_PK1= 0;
      PORTK_PK0= 0;
      DISPLAY_NUMBER=shuma[hundreds];
      break;

    case 3:
      PORTK_PK3= 0;
      PORTK_PK2= 0;
      PORTK_PK1= 1;
      PORTK_PK0= 0;
      DISPLAY_NUMBER=shuma[tens];
      break;

    case 4:
      PORTK_PK3= 0;
      PORTK_PK2= 0;
      PORTK_PK1= 0;
      PORTK_PK0= 1;
      DISPLAY_NUMBER=shuma[ones];
      break;
    
    default:
      break;
  }
  
  DisplayUnit++;
  if(5==DisplayUnit)
  {
  	DisplayUnit=1;
  }
}
