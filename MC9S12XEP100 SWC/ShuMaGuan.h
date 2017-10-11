#ifndef _ShuMaGuan_H_
#define _ShuMaGuan_H_
#include <stdint.h>


void ShuMaGuan_init();
void ShuMaGuan_display(uint8_t DisplayUnit,uint8_t number);
void ShuMaGuan_ScanDisplayNumber_split(uint8_t number);
void ShuMaGuan_ScanDisplayNumber(void);




#endif