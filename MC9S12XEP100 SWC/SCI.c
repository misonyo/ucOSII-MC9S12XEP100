#include "SCI.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "PLL.h"


void sci_init(void)
{
	SCI0BD = BUS_CLOCK/16/115200;   //设置SCI0波特率为115200
	SCI0CR1 = 0x00;       //设置SCI0为正常模式，八位数据位，无奇偶校验
	SCI0CR2 = 0b00101100;       //允许发送数据和接受数据,enable interrupts 
}

void sci_send(uint8_t data) 
{
  while(!SCI0SR1_TDRE);       //Transmit Data Register Empty Flag
  SCI0DRL = data;
}

void sci_send_string(uint8_t *putchar) 
{
  while(*putchar!=0x00)       //判断字符串是否发送完毕
  {
   SCI_send(*putchar++);  
  }
}
uint8_t sci_receive(void) 
{
  while(!SCI0SR1_RDRF);          //Receive Data Register Full Flag
  return(SCI0DRL);
}
