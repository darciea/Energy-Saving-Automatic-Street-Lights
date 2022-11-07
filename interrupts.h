#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

unsigned int minute = 0;
unsigned int hour = 0;
unsigned int current_day_hour = 0;
unsigned int current_day_min = 0;
unsigned int day_before_hour = 0;
unsigned int day_before_min = 0;



#endif
