#ifndef _PLL_H_
#define _PLL_H_
#include <stdint.h>

#define BUS_CLOCK		   32000000	   //总线频率
#define OSC_CLOCK		   16000000	   //晶振频率


void pll_init(void);
					

#endif