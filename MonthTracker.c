/*
 * File:   MonthTracker.c
 * Author: Matti
 *
 * Created on 07 November 2022, 16:31
 */


#include <xc.h>


     unsigned int check_month(unsigned int month, unsigned int month_day, unsigned int year)
    {
        int leap_year = 0;
        
        if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8|| month == 10 || month == 12) && month_day == 32){
            //for Jan, Mar, May, July, Aug, Oct, Dec, once the date of the month exceed 31, reset date and move to the next month
            month_day = 1;
            month++;
            if (month == 13){
                month = 1;
                year++;
            }
        }
        
        if ((month == 4 || month == 6 || month == 9 || month == 11) && month_day == 31){
            //for April, June, Sept, once the date of the month exceed 30, reset date and move to the next month
            month_day = 1;
            month++;
        }
        
              
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {leap_year = 1;} //check if the year is a leap year
        else {leap_year = 0;} 
        
        if (month == 2 && leap_year == 1){
            //if it is a leap year, let there be 29 days in feb
            if (month_day == 30){
                month_day = 1;
                month++;
            }
            else if (month == 2 && leap_year ==0){ // if not a leap year, keep 28 days in feb
            if (month_day == 29){
                month_day = 1;
                month++;
                }  
        }
        
            
            
        }
            
        return (month, month_day);
        
    }
 
