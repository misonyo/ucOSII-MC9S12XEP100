#ifndef _LED_H_
#define _LED_H_
#include <stdint.h>

typedef enum
{
 	eLed_ON=0,
 	eLed_OFF,
 	eLed_1HZ,
 	eLed_2HZ,
 	eLed_3HZ,
 	eLed_complex,
 	eLed_StatusMax,	
}led_status_t;

void led_periodic_task(void);
void led_init(void);
void led_status_change(int8_t led_number,led_status_t status);

#endif