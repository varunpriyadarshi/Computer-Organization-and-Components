/* mipslabwork.c

   This file written 2015 by F Lundevall

   This file should be changed by YOU! So add something here:

   This file modified 2015-12-24 by Ture Teknolog 

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int mytime = 0x5957;

volatile int* porte=0xbf886110;

int timeoutcount=0;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{

  /** ASSIGNMENT 1 */
  volatile int *triseclr = (volatile int*)0xbf886104; //checked the value of TRISECLR in header 
  //set 0-7 bits to ones in TRISECLR so that those bits in TRISE will become 0 (output)
  //other bits wont be changed when using TRISECLR
  *triseclr = 0xFF; 


  TRISDSET = 0xFE0; //bits 11-5 set to 1 so that they will be 0 in TRISD (output)

   /** ASSIGNMENT 2 */

  //stop the clock until the init is finished
  T2CON = 0x0; //set the first bit to 0 to stop

  /*
    bit6-4 = 11 because we want 1:256 scaling on the clock
  */

  T2CONSET = 0x070; //0111 0000 we set the prescaling. 
  //T2CONSET will not alter all positions with 0

  TMR2 = 0x0; //clear the timer
  PR2 = 0x7A12; //31250 = 80MHz/256/10. So the timer will reset 10 times each second

  T2CONSET = 0x8000; //set first bit to 1. 8 = 1000 start the cloc


  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{

  int buttonsPressed=getbtns();

  char BTN2=buttonsPressed&0x01;
  char BTN3=(buttonsPressed>>1)&0x01;
  char BTN4=(buttonsPressed>>2)&0x01;

  int switchValue=getsw();

  //

  //0x1230 -> 0x1430
  /*
      0x0400
      0x1030
  */

  if(BTN2){
    //0100
    int newDigit = switchValue * 16;
    int maskedCurrentTime = mytime & 0xFF0F; //mask away the third position
    mytime = newDigit + maskedCurrentTime; //put the new digit in the correct place

  }

  else if(BTN3){

    int newDigit = switchValue * 16 * 16;
    int maskedCurrentTime = mytime & 0xF0FF; //mask away the third position
    mytime = newDigit + maskedCurrentTime; //put the new digit in the correct place

  }

  else if(BTN4){

    int newDigit = switchValue * 16 * 16 * 16;
    int maskedCurrentTime = mytime & 0x0FFF; //mask away the third position
    mytime = newDigit + maskedCurrentTime; //put the new digit in the correct place

  }

  /* ASSINGMENT 2 */

  //IN TABLE 4-4 on page 53 we found that T2IF is bit 8 in IFS0  

  if(IFS(0) & 0x100){
    timeoutcount++;
    IFSCLR(0)=0x100; //clear only T2IF bit (bit 8) 
  }

  if(timeoutcount==10){
    time2string( textstring, mytime );
    display_string( 3, textstring );
    display_update();
    tick( &mytime );
    display_image(96, icon);

    

    timeoutcount=0;
    *porte=(*porte+1)%256; //add one to PORTE which will change the LEDs so they shine in binary
  }


  
  
}

/**
• How is the time-out event-flag checked?
  Check T2IF bit in IFS0. T2IF is timer 2 intrerrupt flag.


• When the time-out event-flag is a "1", how does your code reset it to "0"?
  
  IFSCLR(0)=0x100; //clear only T2IF bit (bit 8)
  when we use clear we set the bit to 0 on the position with 1.
  the rest of the bits are left untouched

• What would happen if the time-out event-flag was not reset to "0" by your code? Why?
  
  timeoutcount++; will be called each tick because the if-conditions are met.
  The clock would tick fast because it would display everything each 10th iteration without delay.

• Make a rough estimate of how many times per second your code 
  checks the time-out event- flag: 
  1, 10, 100, 1000, 10 000, 100 000, a million, 10 million, 100 million times? 
  How did you calculate your answer?

  We calculated about 10 million.
  In main() the program initiates and then runs while(1) labwork()

   EACH ITERATION WE DO:
    1) jump to labwork
    2) get IFS(0) 
    3) shift it 
    4) compare to 1.

  IFS(0) has some operation underneath
  Then when both IFS(0) == 1 and timeoutcount==10 we print the time. 
  THIS HAPPENS EXTREMELY RARELY.

  Therefore 80/~10


  

• Does your code write to T2CON, T2CONSET, and/or T2CONCLR? 
  How many times do you write to each of these device-registers? Why?

  T2CON = 1 time, T2CONSET = 2 times, T2CONCLR = 0 times

  We use T2CON to stop the clock when we init
  T2CONSET = 0x070; //0111 0000 we set the prescaling.
  T2CONSET = 0x8000; //set first bit to 1. 8 = 1000 start the clock

• Which device-register (or registers) must be written to define 
  the time between time-out events? 
  Describe the function of that register (or of those registers).

  FRÅGAAA!"!!"!!!
  menas perioden?
  PR2 = 0x7A12;?

• If you press BTN3 quickly, does the time update reliably? 
  Why, or why not? If not, would that be easy to change? If so, how?

  Not a delay. Respond exactly when you press.

*/
