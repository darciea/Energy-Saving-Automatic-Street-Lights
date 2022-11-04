// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "LEDarray.h" 
#include "interrupts.h"
#include "comparator.h"
#include "timers.h"
#include "ADC.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

//global variable 'minute' defined in interrupts.h


void main(void) {

    Timer0_init();
    LEDarray_init();
    Interrupts_init();
    LEDarray_init();
    Comp1_init();
    Light_init();
    ADC_init();
    
    while(1){
        
        if (minute == 60) {
            hour++;
            minute = 0;
            if (hour == 24){hour = 0;}
        }
        
        LEDarray_disp_bin(hour); //display current timer value (most significant bits only) on LED array
    }
}