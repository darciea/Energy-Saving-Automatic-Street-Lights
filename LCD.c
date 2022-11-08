#include <xc.h>
#include <stdio.h>
#include "LCD.h"

/************************************
 * Function to toggle LCD enable bit on then off
 * when this function is called the LCD screen reads the data lines
************************************/
void LCD_E_TOG(void)
{
	LCD_E = 1;//turn the LCD enable bit on
	__delay_us(2); //wait a short delay
	LCD_E = 0;//turn the LCD enable bit off again
}

/************************************
 * Function to set the 4-bit data line levels for the LCD
************************************/
void LCD_sendnibble(unsigned char number)
{

	if (number & 0b0001) {LCD_DB4 = 1;} else {LCD_DB4 = 0;}//set the data lines here (think back to LED array output)
    if (number & 0b0010) {LCD_DB5 = 1;} else {LCD_DB5 = 0;}
    if (number & 0b0100) {LCD_DB6 = 1;} else {LCD_DB6 = 0;}
    if (number & 0b1000) {LCD_DB7 = 1;} else {LCD_DB7 = 0;}
    
    LCD_E_TOG();			//toggle the enable bit to instruct the LCD to read the data lines
    __delay_us(5);      //Delay 5uS
}


/************************************
 * Function to send full 8-bit commands/data over the 4-bit interface
 * high nibble (4 most significant bits) are sent first, then low nibble sent
************************************/
void LCD_sendbyte(unsigned char Byte, char type)
{
    if (type) {LCD_RS = 1;} else {LCD_RS = 0;} // set RS pin whether it is a Command (0) or Data/Char (1) using type argument
    LCD_sendnibble((Byte >> 4)& 0b1111);// send high bits of Byte using LCDout function
    LCD_sendnibble(Byte & 0b00001111);// send low bits of Byte using LCDout function
	
    __delay_us(50);               //delay 50uS (minimum for command to execute)
}

/************************************
 * Function to initialise the LCD after power on
************************************/
void LCD_Init(void)
{

    TRISCbits.TRISC6 = 0;//Define LCD Pins as Outputs and
    TRISCbits.TRISC2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB2 = 0;
    TRISEbits.TRISE3 = 0;
    TRISEbits.TRISE1 = 0;
    
    LCD_RS = 0;//set all pins low (might be random values on start up, fixes lots of issues)
    LCD_E = 0;
    LCD_DB4 = 0;
    LCD_DB5 = 0;
    LCD_DB6 = 0;
    LCD_DB7 = 0;

    //Initialisation sequence code
	__delay_ms(50);// follow the sequence in the GitHub Readme picture for 4-bit interface.
	LCD_sendnibble(0b0011);// first Function set should be sent with LCD_sendnibble (the LCD is in 8 bit mode at start up)
    __delay_us(45);
    LCD_sendnibble(0b0010);// after this use LCD_sendbyte to operate in 4 bit mode
    __delay_us(45);
    LCD_sendbyte(0b00101000,0);
    __delay_us(45);
    
    LCD_sendbyte(0b00001000,0);//Display ON/OFF control
    __delay_us(45);
    LCD_sendbyte(0b00000001,0); //Display Clear
    __delay_ms(2);
    LCD_sendbyte(0b0000110,0);

    
    LCD_sendbyte(0b00001100,0); //unsure how to turn on
	//remember to turn the LCD display back on at the end of the initialisation (not in the data sheet)
}
/************************************
 * Function to clear the screen
************************************/
void LCD_clear(void)
{
    LCD_sendbyte(0b00000001,0); //Display Clear
    __delay_ms(2);
}

/************************************
 * Function to set the cursor to beginning of line 1 or 2
************************************/
void LCD_setline (char line)
{
    if (line == 1) {LCD_sendbyte(0x80,0);};//Send 0x80 to set line to 1 (0x00 ddram address)
    if (line == 2) {LCD_sendbyte(0xC0,0);}//LCD_sendbyte(0xC0,1);//Send 0xC0 to set line to 2 (0x40 ddram address)
}

/************************************
 * Function to send string to LCD screen
************************************/
void LCD_sendstring(char *string)
{
	while(*string != 0) {//code here to send a string to LCD using pointers and LCD_sendbyte function
        LCD_sendbyte(*string++,1);
    }
}    

/************************************
 * Function to send string to LCD screen
************************************/
void LCD_scroll(void)
{
	LCD_sendbyte(0b00011000,0);//code here to scroll the text on the LCD screen
    
}

/************************************
 * Function takes a ADC value and works out the voltage to 2 dp
 * the result is stored in buf as ascii text ready for display on LCD
 * Note result is stored in a buffer using pointers, it is not sent to the LCD
************************************/
void ADC2String(char *buf, unsigned int ADC_val){
	//code to calculate the inegeter and fractions part of a ADC value
	int int_part = ADC_val/77; //i.e. 255 /3.3V = 77
    int frac_part = (ADC_val*100)/77 - int_part*100;
    sprintf(buf, "Voltage = %d.%02d",int_part, frac_part);
    // and format as a string using sprintf (see GitHub readme)
}

void LDR2String(char *val, unsigned int ADC_val){
	//code to calculate the inegeter and fractions part of a ADC value
	//int int_part = ADC_val/77; //i.e. 255 /3.3V = 77
    //int frac_part = (ADC_val*100)/77 - int_part*100;
    sprintf(val, "getval = %d",ADC_val);
    // and format as a string using sprintf (see GitHub readme)
}