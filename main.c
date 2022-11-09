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
//#include "ADC.h"
#include "MonthTracker.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

//global variables defined in interrupts.h



void main(void) {
   
    //manually initialise variables for when program starts
    unsigned int day = 4; //(0 = sunday,6 = sat)
    unsigned int month = 10; //
    unsigned int month_day = 27;
    unsigned short year = 2022;
    unsigned int isDST = 1; //define whether the clock starts inside (1) or outside (0) of DST
    
    //flags to ensure changes aren't repeated and program accounts for uncommon eventualities 
    unsigned int changed = 0; //indicate whether we are in the hour when the clocks have been shifted back at the end of DST
    unsigned int OneAmToFiveAmFlag = 0; //indicate whether we are between 1 and 5 am
    unsigned int daily_correction = 0; //indicate whether the clock has been shifted back due to the timer being out of sync with the solar clock.
    
    //space made for strings to show variables on the LCD screen
    char datestr[50];
    char yearstr[20];
    

    Timer0_init();
    LEDarray_init();
    Interrupts_init();
    LEDarray_init();
    Comp1_init();
    Light_init();
    //ADC_init();
    LCD_Init();
    
    while(1){
        

        LCD_setline(1); //Set Line 1
        sprintf(datestr, "%d-%d-%d     %d", month_day, month, year, day);
        LCD_sendstring(datestr);
        LCD_setline(2); //Set Line 1
        sprintf(yearstr, "%d:%d", hour, minute);
        LCD_sendstring(yearstr);
        LEDarray_disp_bin(hour); //display hour variable on LED array
        
        if (minute == 60) { //trigger every time minute variable hits 60
            hour++; // increment hour by 1
            LEDarray_disp_bin(hour); //display hour variable on LED array
            minute = 0; //reset minute to 0
            if (hour == 24){ //at the end of the day
                hour = 0; //reset for midnight
                
                 //below is the algorithm used to determine length of daylight using minute and hour values.
                if (daylight_end_min >= daylight_start_min) { //checking if the hour needs to be 'carried over'
                    current_day_min = daylight_end_min - daylight_start_min; 
                    current_day_hour = daylight_end_hour - daylight_start_hour;}
                else {
                    current_day_min = daylight_end_min + (60-daylight_start_min);
                    current_day_hour = daylight_end_hour - daylight_start_hour - 1;}
                
                //now determine the time at which solar noon occurred
                calculated_solar_noon_hour = (daylight_start_hour*60 + daylight_start_min + (current_day_hour*60 + current_day_min)/2)/60; //converts everything into minutes first, adds everything together and then converts back to hours. Makes use of the fact that the / function returns the 'floor' value
                calculated_solar_noon_min = (daylight_start_hour*60 + daylight_start_min + (current_day_hour*60 + current_day_min)/2)%60;
                
                //if calculated solar noon is not equal to 12:00 (or 13:00 during DST), adjust time to correct for this
                if (calculated_solar_noon_hour >= (12 + isDST) && calculated_solar_noon_min != 0 && daily_correction == 0) { //check if the measured solar noon happened after the timer's solar noon. Use the isDST flag (1 or 0) to account for the fact that solar noon is at 13:00 during DST
                    daily_correction = 1; //indicate we will need to add some time again in 'daily correction time'. Date shift is inhibited by this flag
                    //LATDbits.LATD7=1;//only turn this on to use LED to indicate when daily correction time is}
                    hour = 23 - (calculated_solar_noon_hour - 12 - isDST); //shift the hour back by the required amount
                    minute = 60 - calculated_solar_noon_min; //shift the minute back by the required amount
                }
                else if (calculated_solar_noon_hour < (12 + isDST)){ //check if the measured solar noon happened before the timer's solar noon. Use the isDST flag (1 or 0) to account for the fact that solar noon is at 13:00 during DST
                   hour = 11 + isDST - calculated_solar_noon_hour; //shift hour forward by the required amount
                   minute = 60 - calculated_solar_noon_min; //shift minute forward by the required amount
                }
                else if (hour == 0 && daily_correction == 1){daily_correction = 0; // if the hour 0 has been reached as the program is in 'daily correction time', it means daily correction time is over, and the flag needs to be removed 
                    //LATDbits.LATD7=0;//only turn this on to use LED to indicate when daily correction time is}
                }
                if (daily_correction != 1){ //prevent the date shift from happening if we are in 'daily correction time'
                day++; //make it the next day
                month_day++; //increment the date of the month
                check_month(&month, &month_day, &year); //function to ensure the date is incremented correctly
                if (day == 7){day = 0;} //reset the day of the week
                }
                LCD_clear();
                
            }
            LEDarray_disp_bin(hour); //display hour variable on LED array  
        }
              
        if (hour >= 1 && hour < 5){ //check whether we are in the 1-5am zone.
            LATHbits.LATH3=0; // If so, make sure LED is turned off, overwriting everything
            OneAmToFiveAmFlag = 1;} // use flag to indicate we are within the 1am to 5 am region  
        if (hour == 5 && OneAmToFiveAmFlag == 1 && CM1CON0bits.OUT == 1){ // as the time exits the 1am to 5am zone, check that it is still dark, and use the flag...
                LATHbits.LATH3=1; // to trigger the LED coming back on
                OneAmToFiveAmFlag = 0;} // and reset the flag so that it doesn't get turned on continuously
       
        if (day == 0 && month == 3 && hour == 1 && minute == 0 && month_day >=25 && month_day <= 31) {hour++; isDST = 1;} //when daylight savings time starts, add one hour and change flag that tells solar correction program that solar noon is at 1pm
        
        if (day == 0 && month == 10 && hour == 2 && minute == 0 && month_day >=25 && month_day <= 31 && changed == 0) { //when daylight savings time ends, minus 1 hour
            hour--; //go back one hour
            changed = 1; //flag to ensure that the clock doesn't go back again
            isDST = 0; // change flag that tells solar correction program that solar noon is now back at 12pm
        }
        else if (day == 0 && month == 10 && hour == 4 && minute == 0 && month_day >=25 && month_day <= 31 && changed == 1){changed = 0;}
        
    }
}
