# Mini project - Energy saving automatic outside light

## Explanation of our work
To correctly start the program, you must first manually initialise the day (0 = Sunday, 6 = Saturday), the day of the month, the month and the year, and whether we are in DST or not in the main.c file, and the hour and minute should be set in the interrupts.h file.

Demonstration video: https://youtu.be/prrxO2kyVkE 

This program relies on the fact that the sun rises and sets exactly once a day (ie a major eclipse may cause problems), and that it is dark at midnight (ie sunset occurs before midnight). This program should be initiated before the sun rises on the day to avoid difficulties. Potential issues may also arise if the timer is 2 hours or more out of sync with the solar clock on the day when the clocks are changing due to entering or exiting DST.

**Key program files**

**"Main.c"**

Within this file we have intitialised the date and all functions connected to hardware that we are using, e.g. the LED, the LDR, the LCD (optional) and others. We have decided to make our timer overflow every 'minute' (whether this 'minute' is in testing mode or real-time depends on the value we set for the timer overflow, which will be explained in the timers.c/h file'. Every time 60 minutes (i.e. an 'hour'), the minutes reset and the hours are incremented and displayed on the LED array. 

Once we reach the end of the day (24 hours), we will increment the days, both the day of the week (using 0 - 6 as representation) and the day of the month - the relation of this day to the rest of the year will be explained in the 'MonthTracker.c/h file'. Once the end of the week is reached, the day of the week is reset. 

Also at the end of every day (at midnight), the length of daylight is calculated using the dawn and dusk triggered by the interrupt. This is then used to calculate the time at which solar noon occurred. Using the solar noon allows calibration of the timer with the sun.

In theory solar noon should occur at 12:00 every day (and 13:00 during DST). At midnight the time at which the timer was at during measured solar noon is calculated. If this is not 12:00 (or 13:00 during DST), the timer is adjusted accordingly. If the calculated solar noon is after 12:00, the clock was runnning slow, so the difference is just added onto the current time. If the calculated solar noon is before 12:00, the clock was running fast, and so it needs to jump back by the difference; first, 'daily correction time' is initiated, which inhibits the date from increasing. Then the timer is pushed back by the required amount of time, and when this time is over (ie the clock reached midnight again), 'daily correction time' is turned off and the date increases.

Code is then implemented to ensure that between the hours of 1am and 5am, the LED is turned off, regardless of whether there is ambient light or not. As 5am occurs, the code checks that it is still dark (i.e. the LED is required) before turning the light on, so that the light is not turned on if dawn occurs before 5am and the streetlight is not needed.

The program then checks for if it is Daylight Savings Time: 
  Daylight Savings time starts on the last Sunday of March 1am, where this Sunday must fall within the 25th and the 31st, and so an hour is skipped (i.e. the clocks go forward)
   Daylight Savings time ends on the last Sunday of October 2am, so it is set to an hour behind, with a flag introduced to ensure that when it reaches the same time again, the hour is not changed but the flag is reset a couple hours later ready for the next year.
   
We have additionally displayed the date on the LCD array to view the days and how it changes, however this is an optional feature.
   
**"Timers.c/h"**

This file initialises the timer to run and determines whether we are in testing mode or real time mode. In testing mode, the timer prescaler is set to 1:4 and so one hour corresponds to 1 second. In real time mode, we want it to overflow every 60 seconds, so the prescaler will be set to 16384, where the timer overflows every 67.107 seconds. To ensure that it overflows every 60 seconds, we calculate that this prescaler counts to 58593.75 every 60 seconds, so we will initialise our TMR0H:TMR0L to the binary equivalent of 65535 - 58593.75 = 6941.25 ~ 6941. 


**"Comparator.c/h"**

In this file, the LED light is initialised, as well as the comparator, set to recognise ambient light as one input. Here we have also set the comparator to trigger the interrupt on both the rising and falling edge.

**"Interrupts.c/h"**

Here we have declared global variables that are changed in the interrupt and variables required to track the dawn and the dusk in hours and minutes. We have two interrupts working, one being the timer interrupt that increments the minute variable every time it overflows. The other checks the state of the comparator once triggered (whether it went from light to dark, or dark to light) and logs time in hours and minutes accordingly to be processed later. It is also set to turn on the lights when it is dark at all hours except for 1am - 5am.

**"MonthTracker.c/h"**

Due to the code iterating the dates being quite bulky, they have been stored in another file and put into a function where arguments are passed. In order to avoid the use of global variables, this function utilises pointers, receiving the address of the day of the month, the month and the year and changing the pointers so that multiple variables can be adjusted as necessary. This function checks which month it is and resets the date of the month according to how many days that month should have e.g. October has 31 days, so when it is incremented to 32, the date is reset to the first and the month moves on to the next one. In December, the year is then incremented following the end of the month. There is also a flag introduced to check whether it is a leap year (if the year is divisible by 4, except in years that are divisible by 100, in which case the year is checked for if it is divisible by 400) and if it is a leap year February has 29 days, otherwise it has 28.



## Learning outcomes

The principal learning objectives for this project are:

- Implement a working microprocessor based system to achieve a more complex real world task
- Develop your ability to independently plan, organise and structure your code 
- Improve your grasp of the C language and writing your own functions

## Brief

Outside lights that respond to ambient light levels are commonplace (i.e. street lights, porch lights, garden lights etc). These types of lights switch on at dusk and then turn off at dawn. However, energy and money can be saved if these lights are switched off during the small hours of the morning (for example, between 1am and 5am), when there are very few people around. Many councils in the UK have implemented/trialled this idea for street lights (https://www.bbc.co.uk/news/uk-england-derbyshire-16811386). Your task is to use the knowledge of microcontrollers and hardware that you have gained in labs 1-3 from this module to develop a fully automated solution.

## Specification
Design and program a device that meets the following requirements:

1. Monitors light level with the LDR and turns on an LED in low light conditions (i.e. night-time) and off in bright conditions (i.e. daytime)
1. Displays the current hour of day on the LED array in binary
1. Turns the light off between approx. 1am and 5am
1. Adjusts for daylight savings time
1. Maintain synchronicity with the sun indefinitely
1. Be fully automatic (requires zero maintenance after installation)

Please use this GitHub repo to manage your software development and submit your mini project code.

## Supplementary information and help
At first the task may seem quite straightforward but there are several points that often prove more tricky. The first is how to test code during development? You could test in real world conditions but you would be limited to one test cycle per day and this would severely slow down your development and debugging progress. To get around this you could implement a "testing mode" and pretend that a day lasts 24 seconds. This could be done using a #define directive to switch between "normal" and "testing" modes for your code.

Adjusting for daylight savings time is not too tricky. The clocks always change (in the UK) on the last Sunday in March (they go forward an hour) and the last Sunday in October (they go back an hour). One method of achieving this is to initialise what day it is when device is first switched on (using manual input) and then keep track of the days that pass and what the day of the week it is. Another method might be to automatically figure out what time of year it is (see below). Also don't forget about leap years! 

No clock is perfect, they can all run slightly fast/slow and can by influenced be external factors such as temperature. Ultimately this will result in drift over time and eventually the time will drift so far out of sync with real time that it is meaningless. For the purposes of our device the main requirement is that it remains in sync with the sun. You could use light from the sun to keep your clock in sync. Although the length of daylight varies considerably during the year, the midpoint between dusk and dawn only varies by a few minutes. This is termed solar midnight approx. 12am or solar noon approx. 12pm. One method of staying in sync with the sun is to use the LDR and record/calculate when these times occur and adjust your clock accordingly. The length of daylight also tells us information about what time of year it is and can be used to help us know when to adjust for daylight savings time.

![Day length](gifs/day-length-london.jpg)
http://wordpress.mrreid.org/2010/10/31/why-change-the-clocks/








