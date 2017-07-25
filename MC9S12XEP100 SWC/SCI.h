#ifndef _SCI_H_
#define _SCI_H_
#include <stdint.h>


void sci_init(void);
void sci_send(uint8_t data);
void sci_send_string(uint8_t *putchar);
uint8_t sci_receive(void);
					

#endif