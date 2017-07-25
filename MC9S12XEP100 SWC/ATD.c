#include "ATD.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

void atd_init(void)
{
	ATD0CTL2_AFFC=1;	//启动A/D转换,快速清零,禁止中断,Changes all ATD conversion complete flags to a fast clear sequence
	ATD0CTL1_SRES=2;  //选用12位模数转换
	ATD0CTL3 = 0x88;   //每次转换1个通道
	ATD0CTL4 = 0x07;   //,AD模块时钟频率为2MHz
}

uint16_t atd_capture(uint8_t channel) 
{
	 uint16_t AtdResult;
	 if(channel==0)
	    ATD0CTL5 = 0x00;   //select the analog input channel AN0
	 if(channel==1)
	    ATD0CTL5 = 0x01;   //select the analog input channel AN1
	 while(!ATD0STAT2_CCF0);	//A conversion complete flag is set at the end of each conversion in a sequence
	 AtdResult = ATD0DR0;	//ATD Conversion Result Registers,The A/D conversion results are stored in 16 result registers
	 return(AtdResult);
}