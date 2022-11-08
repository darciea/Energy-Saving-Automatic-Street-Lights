// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "LEDarray.h" 
#include "LCD.h"
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
    unsigned int month = 2; //
    unsigned int month_day = 27;
    unsigned short year = 2020;
    unsigned int changed = 0;
    unsigned int OneAmToFiveAmFlag = 0;
    char datestr[50];
    char yearstr[20];
    

    Timer0_init();
    LEDarray_init();
    Interrupts_init();
    LEDarray_init();
    Comp1_init();
    Light_init();
    ADC_init();
    LCD_Init();
    
    while(1){
        

        LCD_setline(1); //Set Line 1
        sprintf(datestr, "%d / %d", month_day, month);
        LCD_sendstring(datestr);
        LCD_setline(2); //Set Line 1
        sprintf(yearstr, "%d / %d", day, year);
        LCD_sendstring(yearstr);
   
        
        if (minute == 60) { //trigger every time minute variable hits 60
            hour++; // increment hour by 1
            minute = 0; //reset minute to 0
            if (hour == 24){ //at the end of the day
                hour = 0; //reset for midnight
                day++; //make it the next day
                month_day++; //increment the date of the month
                check_month(&month, &month_day, &year); //function to ensure the date is incremented correctly
                if (day == 7){day = 0;} //reset the day of the week
                LCD_clear();

                
                 //below is the algorithm used to determine length of daylight using minute and hour values.
                if (daylight_end_min >= daylight_start_min) {
                    current_day_min = daylight_end_min - daylight_start_min;
                    current_day_hour = daylight_end_hour - daylight_start_hour;}
                else {
                    current_day_min = daylight_end_min + (60-daylight_start_min);
                    current_day_hour = daylight_end_hour - daylight_start_hour - 1;
                }
                //now determine the time at which solar noon occurred
                calculated_solar_noon_hour = (daylight_start_hour*60 + daylight_start_min + current_day_hour*60 + current_day_min)/60;
                calculated_solar_noon_min = (daylight_start_hour*60 + daylight_start_min + current_day_hour*60 + current_day_min)%60;
                
                //if calculated solar noon is not equal to 12:00, adjust time to correct for this
            } 
        }
              
        if (hour >= 1 && hour < 5){ //check whether we are in the 1-5am zone.
            LATHbits.LATH3=0; // If so, make sure LED is turned off, overwriting everything
            OneAmToFiveAmFlag = 1;} // use flag to indicate we are within the 1am to 5 am region  
        if (hour == 5 && OneAmToFiveAmFlag == 1 && CM1CON0bits.OUT == 1){ // as the time exits the 1am to 5am zone, check that it is still dark, and use the flag...
                LATHbits.LATH3=1; // to trigger the LED coming back on
                OneAmToFiveAmFlag = 0;} // and reset the flag so that it doesn't get turned on continuously
        
        LEDarray_disp_bin(hour); //display hour variable on LED array
        
         
        
        if (day == 0 && month == 3 && hour == 1 && minute == 0 && month_day >=25 && month_day <= 31) {hour++;} //when daylight savings time starts, add one hour
        
        if (day == 0 && month == 10 && hour == 2 && minute == 0 && month_day >=25 && month_day <= 31 && changed == 0) { //when daylight savings time ends, minus 1 hour
            hour--; //go back one hour
            changed = 1; //flag to ensure that the clock doesn't go back again
        }
        else if (day == 0 && month == 10 && hour == 2 && minute == 0 && month_day >=25 && month_day <= 31 && changed == 1){changed = 0;}
        
    }
}