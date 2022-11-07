#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

unsigned int hour = 0;
unsigned int minute = 0;

unsigned int current_day_hour = 0;
unsigned int current_day_min = 0;
unsigned int daylight_start_hour = 0;
unsigned int daylight_start_min = 0;
unsigned int daylight_end_hour = 0;
unsigned int daylight_end_min = 0;
unsigned int calculated_solar_noon_hour;
unsigned int calculated_solar_noon_min;



#endif
