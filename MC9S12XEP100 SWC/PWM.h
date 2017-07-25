#ifndef _PWM_H_
#define _PWM_H_
#include <stdint.h>


void pwm_init(void);
void pwm_on_off(uint8_t OnOff,uint8_t bit);
void pwm_SetPeriodAndDuty(uint8_t channel,uint8_t period,uint8_t duty);




#endif