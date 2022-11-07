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
#include "MonthTracker.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

//global variables defined in interrupts.h



void main(void) {
   
    //manually initialise variables for when program starts
    unsigned int day = 2; //(0 = sunday,6 = sat)
    unsigned int month = 11; //
    unsigned int month_day = 29;
    unsigned int year = 2022;
    unsigned int changed = 0;
    
    

    Timer0_init();
    LEDarray_init();
    Interrupts_init();
    LEDarray_init();
    Comp1_init();
    Light_init();
    ADC_init();
    
    while(1){
        
        if (minute == 60) { //trigger every time minute variable hits 60
            hour++; // increment hour by 1
            minute = 0; //reset minute to 0
            if (hour == 24){ //at the end of the day
                hour = 0; //reset for midnight
                day++; //make it the next day
                month_day++; //increment the date of the month
                month, month_day = check_month(month, month_day, year); //function to ensure the date is incremented correctly
                if (day == 7){day = 0;} //reset the day of the week 
                
                
                 //below is the algorithm used to determine length of daylight using minute and hour values.
                if (daylight_end_min >= daylight_start_min) {
                    current_day_min = daylight_end_min - daylight_start_min;
                    current_day_hour = daylight_end_hour - daylight_start_hour;}
                else {
                    current_day_min = daylight_end_min + (60-daylight_start_min);
                    current_day_hour = daylight_end_hour - daylight_start_hour - 1;
                }
                //now determine the time at which solar noon occurred
                calculated_solar_noon_hour = daylight_start_hour + (daylight_end_hour - daylight_start_hour)*0.5;
            } 
        }
              
        if (hour >= 1 && hour < 5){LATHbits.LATH3=0;} //check whether we are in the 1-5am zone. If so, make sure LED is turned off
        
        LEDarray_disp_bin(hour); //display hour variable on LED array
        
         
        
        if (day == 0 && month == 3 && hour == 1 && minute == 0 && month_day >=25 && month_day <= 31) {hour++;} //when daylight savings time starts, add one hour
        
        if (day == 0 && month == 10 && hour == 2 && minute == 0 && month_day >=25 && month_day <= 31 && changed == 0) { //when daylight savings time ends, minus 1 hour
            hour--; //go back one hour
            changed = 1; //flag to ensure that the clock doesn't go back again
        }
        else if (day == 0 && month == 10 && hour == 2 && minute == 0 && month_day >=25 && month_day <= 31 && changed == 1){changed = 0;}
        
    }
}