#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1000; // 1:256 since (1/65535 * 64MHz)/4 = 244 ~ 256 
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    // it's a good idea to initialise the timer registers so we know we are at 0
    TMR0H=0b00001011;            //TMR0H:TMR0L so that the entire value is 3035
    TMR0L=0b11011011;  //start at 3035 as that is the discrepancy between 1:244 and 1:256
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * Function to return the full 16bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
unsigned int get16bitTMR0val(void)
{
	//add your code to get the full 16 bit timer value here
    unsigned int count = TMR0L; //read from TMR0L first
    count = count + (TMR0H<<8); //shift TMR0H to the higher bits and then combine into a 16 bit number
    return (count>>8); // give us the most significant bits
    
    
}
