#include "msp430.h"
#include "stdlib.h"

int count=0;                    // Incrememnts upon button press
const char *patternOut[3];      // Array to hold LED output pattern
int outLength;                  // Number of elements in patternOut
const char *patternIn[3];       // Array to hold BTN user input pattern
int randPtrn;                 // Used in selecting random pattern to output

void configureClocks(void)
{
  CSCTL0 = CSKEY;                 	// Unlock
  CSCTL2 |= SELA__LFXTCLK;	// Defaults at 32,768
  CSCTL3 |= DIVA__1;              	// Divide by one
}

void configurePorts(void)
{
  PMMCTL0 = PMMPW;		//PMM password to unlock pins
  PM5CTL0 &= ~LOCKLPM5;		//Clear locked  pins
  P1DIR |= BIT0;			//Set P1.0 to output
  P9DIR |= BIT7;			//Set P9.7 to output
  P1DIR &= ~BIT1;			//Set P1.1 to input
  P1DIR &= ~BIT2;			//Set P1.2 to input
  P1OUT &= ~BIT0;			//Turn P1.0 off
  P9OUT &= ~BIT7;			//Turn P9.7 off
  P1REN |= BIT1;		        //Enable pullup/pulldown resistors on P1.1, P1.2
  P1REN |= BIT2;		        //Enable pullup/pulldown resistors on P1.1, P1.2
  P1IE |= BIT1;			        //P1.1 interrupt enabled
  P1IE |= BIT2;			        //P1.2 interrupt enabled
  P1IES |= BIT1;			//P1.1 interrupt edge enable (high-low trans)
  P1IES |= BIT2;			//P1.2 interrupt edge enable (high-low trans)
  P1IFG &= ~BIT1;			//Clear interrupt flag for P1.1
  P1IFG &= ~BIT2;			//Clear interrupt flag for P1.2
  P1OUT |= BIT1;
  P1OUT |= BIT2;
  
}

void choosePattern(void)
{
  randPtrn = rand() % 3;
  //randPtrn = 1;
  
  if(randPtrn == 0)
  {
    patternOut[0] = "L";
    patternOut[1] = "R";
    patternOut[2] = "L";
  }
  else if (randPtrn == 1)
  {
    patternOut[0] = "L";
    patternOut[1] = "L";
    patternOut[2] = "R"; 
  }
  else if (randPtrn == 2)
  {
    patternOut[0] = "R";
    patternOut[1] = "R";
    patternOut[2] = "R"; 
  }
}

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;		//Turn off watchdog timer
  configurePorts();		        //Assume P1.1 is an pull-down input (button) and
  //P1.0 is an output (RED LED)
  configureClocks();
  
  //choosePattern();
  
  __enable_interrupt();		        //Set GIE bit
  
  
  // Set output pattern to be compared to input
  //  patternOut[0] = "L";
  //  patternOut[1] = "R";
  //  patternOut[2] = "L";
  
  outLength = sizeof(patternOut) / sizeof(patternOut[0]);
  
  
  while(1){}
}

#pragma vector=PORT1_VECTOR		//Vector definition in msp430r6989.h
__interrupt void Port_1(void) 
{
  __delay_cycles(1000);			//Need to debounce (first delay)
  if((P1IN & BIT1) != BIT1)	        //Check if LEFT button is still depressed
  {	
    P1IFG &= ~BIT1;			//Clear flag
    choosePattern();            // Choose random output pattern
    
    if(count == 0)              //First button presses initializes output pattern
    {
      if(randPtrn == 0)
      {
        P1OUT ^= BIT0;            //Toggle left LED (on)  LEFT
        __delay_cycles(500000);
        P1OUT ^= BIT0;            //Toggle left LED (off)
        P9OUT ^= BIT7;	        //Toggle right LED (on) RIGHT
        __delay_cycles(500000);
        P1OUT ^= BIT0;            //Toggle left LED (on)         LEFT
        P9OUT ^= BIT7;	        //Toggle right LED (off)
        __delay_cycles(500000);
        P1OUT ^= BIT0;            //Toggle left LED (off)  
      }
      else if (randPtrn == 1)
      {
        P1OUT ^= BIT0;            //Toggle left LED (on)  LEFT
        __delay_cycles(500000);
        P1OUT ^= BIT0;            //Toggle left LED (off)
        __delay_cycles(500000);
        P1OUT ^= BIT0;	        //Toggle left LED (on) LEFT
        __delay_cycles(500000);
        P1OUT ^= BIT0;            //Toggle left LED (off)
        P9OUT ^= BIT7;	        //Toggle right LED (on)    RIGHT
        __delay_cycles(500000);
        P9OUT ^= BIT7;            //Toggle right LED (off)  
      }
      else if (randPtrn == 2)
      {
        P9OUT ^= BIT7;	        //Toggle right LED (on) RIGHT
        __delay_cycles(500000);
        P9OUT ^= BIT7;            //Toggle right LED (off)  
        __delay_cycles(500000);
        P9OUT ^= BIT7;            //Toggle right LED (on)  RIGHT
        __delay_cycles(500000);
        P9OUT ^= BIT7;            //Toggle right LED (off)  
        __delay_cycles(500000);
        P9OUT ^= BIT7;            //Toggle right LED (on)  RIGHT
        __delay_cycles(500000);
        P9OUT ^= BIT7;            //Toggle right LED (off)  
      }
      
      
      
      //      P1OUT ^= BIT0;            //Toggle left LED (on)  LEFT
      //      __delay_cycles(500000);
      //      P1OUT ^= BIT0;            //Toggle left LED (off)
      //      P9OUT ^= BIT7;	        //Toggle right LED (on) RIGHT
      //      __delay_cycles(500000);
      //      P1OUT ^= BIT0;            //Toggle left LED (on)
      //      P9OUT ^= BIT7;	        //Toggle right LED (off)
      //      __delay_cycles(500000);
      //      P1OUT ^= BIT0;            //Toggle left LED (on)  LEFT
      
      
      count++;                  //Increase count to signify the button has already been pushed once
    }
    
    else if(count >= 1)
    {
      P1OUT ^= BIT0;            //Toggle left LED (on)  LEFT
      __delay_cycles(300000);
      P1OUT ^= BIT0;            //Toggle left LED (off)  LEFT
      
      patternIn[count-1] = "L";
      count++;                          //Increase count
    }
    
  }
  else if ((P1IN & BIT2) != BIT2)     //Check if RIGHT button is still depressed
  {
    P1IFG &= ~BIT2;			//Clear flag
    P9OUT ^= BIT7;            //Toggle left LED (on)
    __delay_cycles(300000);
    P9OUT ^= BIT7;            //Toggle right LED (off)  LEFT
    
    patternIn[count-1] = "R";
    count++;
  }
  
  if(count == (outLength + 1))
  {
    
    // Error check user input to output
    // if match, reset
    if(patternIn[0] == patternOut[0] &
       patternIn[1] == patternOut[1] &
         patternIn[2] == patternOut[2])
    {
      P1OUT |= BIT0;			//Turn P1.0 on
      P9OUT |= BIT7;			//Turn P9.7 on
      count = 0;
      patternIn[0] = 0;
      patternIn[1] = 0;
      patternIn[2] = 0;
      
      // Signify win 
      __delay_cycles(300000);
      P1OUT ^= BIT0;			//Turn P1.0 off
      P9OUT ^= BIT7;			//Turn P9.7 off
      __delay_cycles(300000);
      P1OUT ^= BIT0;			//Turn P1.0 on
      P9OUT ^= BIT7;			//Turn P9.7 on
      __delay_cycles(300000);
      P1OUT ^= BIT0;			//Turn P1.0 off
      P9OUT ^= BIT7;			//Turn P9.7 off
      __delay_cycles(300000);
      P1OUT ^= BIT0;			//Turn P1.0 on
      P9OUT ^= BIT7;			//Turn P9.7 on
      __delay_cycles(300000);
      P1OUT ^= BIT0;			//Turn P1.0 off
      P9OUT ^= BIT7;			//Turn P9.7 off
    }
    else{ //incorrect user input
      P1OUT |= BIT0;			//Turn P1.0 on
      P9OUT |= BIT7;			//Turn P9.7 on
      count = 0;
      patternIn[0] = 0;
      patternIn[1] = 0;
      patternIn[2] = 0;
      
      //Signfy lose
      __delay_cycles(300000);
      P1OUT ^= BIT0;			//Turn P1.0 off
      P9OUT ^= BIT7;			//Turn P9.7 off
    }
  }
}
